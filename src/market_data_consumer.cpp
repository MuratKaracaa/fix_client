#include "market_data_consumer.h"
#include "app_config_loader.h"
#include "constants.h"
#include <iostream>

MarketDataConsumer::MarketDataConsumer(moodycamel::ConcurrentQueue<AppMarketData> &market_data_queue, size_t batch_size, size_t thread_count)
    : market_data_queue(market_data_queue),
      batch_size(batch_size),
      thread_count(thread_count)
{
    consumer_tokens.reserve(thread_count);
    connections.reserve(thread_count);

    thread_pool = std::make_unique<BS::thread_pool>(thread_count);

    for (size_t i = 0; i < thread_count; ++i)
    {
        consumer_tokens.emplace_back(market_data_queue);
        connections.push_back(pqxx::connection(database_connection_string));
    }

    bool is_all_connections_open = std::all_of(connections.begin(), connections.end(), [](const pqxx::connection &conn)
                                               { return conn.is_open(); });
    if (!is_all_connections_open)
    {
        throw std::runtime_error("Failed to connect to database");
    }
}

void MarketDataConsumer::start()
{
    for (size_t i = 0; i < thread_count; ++i)
    {
        thread_pool->submit_task([this, i]()
                                 {
            moodycamel::ConsumerToken &consumer_token = consumer_tokens[i];
            std::vector<AppMarketData> app_market_data(batch_size);
            pqxx::connection &connection = connections[i];
            while (global_market_data_consumer_running.load(std::memory_order_relaxed))
            {
                size_t size = market_data_queue.try_dequeue_bulk(consumer_token, app_market_data.data(), batch_size);
                if (size > 0)
                {
                    process_messages(app_market_data, connection, size);
                }
                else
                {
                    std::this_thread::sleep_for(std::chrono::milliseconds(100));
                }
            } });
    }
}

void MarketDataConsumer::stop()
{
    global_market_data_consumer_running.store(false, std::memory_order_relaxed);
    thread_pool->wait();

    std::vector<AppMarketData> app_market_data_vector;
    AppMarketData app_market_data;
    while (market_data_queue.try_dequeue(app_market_data))
    {
        app_market_data_vector.emplace_back(std::move(app_market_data));
    }

    process_messages(app_market_data_vector, connections[0], app_market_data_vector.size());

    for (pqxx::connection &connection : connections)
    {
        connection.close();
    }
    connections.clear();
    consumer_tokens.clear();
}

void MarketDataConsumer::process_messages(std::vector<AppMarketData> &app_market_data, pqxx::connection &connection, size_t count)
{
    try
    {
        std::unordered_map<std::string, std::pair<std::string, double>> latest_price_map;

        for (size_t i = 0; i < count; ++i)
        {
            AppMarketData item = std::move(app_market_data[i]);
            std::string stock_symbol = item.get_stock_symbol();
            std::string timestamp = item.get_timestamp();
            double latest_price = item.get_latest_price();

            if (latest_price_map.contains(stock_symbol))
            {
                auto &pair = latest_price_map[stock_symbol];
                if (timestamp > pair.first)
                {
                    pair.first = timestamp;
                    pair.second = latest_price;
                }
            }
            else
            {
                latest_price_map[stock_symbol] = std::make_pair(timestamp, latest_price);
            }
        }

        pqxx::work work(connection);

        std::string final_query = prepare_update_stock_and_create_outbox_query(work, latest_price_map);
        pqxx::result result = work.exec(final_query);
        work.commit();
    }
    catch (const std::exception &e)
    {
        // TODO: Implement dead letter queue
        std::cerr << e.what() << '\n';
    }
}

std::string MarketDataConsumer::prepare_update_stock_and_create_outbox_query(pqxx::work &work, const std::unordered_map<std::string, std::pair<std::string, double>> &latest_price_map)
{
    std::ostringstream query;

    query << "WITH stock_data(symbol, latest_trading_price, latest_trading_price_time_stamp) AS (VALUES";

    bool first = true;

    for (const auto &[stock_symbol, pair] : latest_price_map)
    {
        if (!first)
        {
            query << ",";
        }
        query << "(" << work.quote(stock_symbol) << "," << work.quote(pair.second) << "," << work.quote(pair.first) << ")";
        first = false;
    }

    query << "), updated_rows AS ("
             "UPDATE stocks "
             "SET latest_trading_price = stock_data.latest_trading_price::NUMERIC, "
             "    latest_trading_price_time_stamp = stock_data.latest_trading_price_time_stamp::TIMESTAMP "
             "FROM stock_data "
             "WHERE stocks.symbol = stock_data.symbol "
             "  AND stocks.latest_trading_price_time_stamp < stock_data.latest_trading_price_time_stamp::TIMESTAMP "
             "RETURNING stocks.symbol, stocks.latest_trading_price, stocks.latest_trading_price_time_stamp"
             ") "
             "INSERT INTO stock_update_outbox (symbol, latest_trading_price, latest_trading_price_time_stamp) "
             "SELECT symbol, latest_trading_price, latest_trading_price_time_stamp "
             "FROM updated_rows;";

    return query.str();
}