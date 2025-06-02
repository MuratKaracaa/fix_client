#include "market_data_outbox_relay.h"
#include "constants.h"
#include "app_market_data_outbox_message.pb.h"

MarketDataOutboxRelay::MarketDataOutboxRelay(RedisConnector &redis_connector)
    : redis_connector(redis_connector)
{
    connection = pqxx::connection(database_connection_string);
}

void MarketDataOutboxRelay::start()
{
    global_outbox_relay_running.store(true, std::memory_order_relaxed);

    worker_thread = std::thread(&MarketDataOutboxRelay::process_outbox_messages, this);
}

void MarketDataOutboxRelay::stop()
{
    global_outbox_relay_running.store(false, std::memory_order_relaxed);
    worker_thread.join();
}

void MarketDataOutboxRelay::process_outbox_messages()
{
    while (global_outbox_relay_running.load(std::memory_order_relaxed))
    {
        pqxx::work work(connection);
        pqxx::result result = work.exec(fetch_outbox_messages_query);

        std::unordered_map<std::string, std::pair<std::string, double>> latest_data_map;
        std::vector<int> ids;
        ids.reserve(result.size());

        for (const auto &row : result)
        {
            std::string stock_symbol = row["stock_symbol"].as<std::string>();
            std::string timestamp = row["latest_trading_price_time_stamp"].as<std::string>();
            double latest_price = row["latest_trading_price"].as<double>();
            int id = row["id"].as<int>();
            ids.push_back(std::move(id));

            if (latest_data_map.contains(stock_symbol))
            {
                auto &pair = latest_data_map[stock_symbol];
                if (timestamp > pair.first)
                {
                    pair.first = timestamp;
                    pair.second = latest_price;
                }
            }
            else
            {
                latest_data_map[stock_symbol] = std::make_pair(timestamp, latest_price);
            }
        }

        bool publish_result = redis_connector.publish_message_and_add_timeseries_data(latest_data_map);
        if (publish_result)
        {
            work.exec_params(purge_outbox_messages_query, pqxx::params{ids});
            work.commit();
        }
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}