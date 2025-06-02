#include "market_data_outbox_relay.h"
#include "constants.h"
#include "app_market_data_outbox_message.pb.h"
#include "utils.h"

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

        AppMarketDataOutboxMessageList app_market_data_outbox_message_list;
        auto *messages = app_market_data_outbox_message_list.mutable_messages();
        messages->Reserve(latest_data_map.size());

        for (const auto &[stock_symbol, pair] : latest_data_map)
        {
            AppMarketDataOutboxMessage app_market_data_outbox_message;
            app_market_data_outbox_message.set_stock_symbol(stock_symbol);
            app_market_data_outbox_message.set_latest_price(pair.second);
            app_market_data_outbox_message.set_timestamp(pair.first);
            messages->Add(std::move(app_market_data_outbox_message));

            int64_t unix_timestamp = Utils::convert_iso_timestamp_to_unix_timestamp(pair.first);
            redis_connector.append_timeseries_command(stock_symbol.c_str(), unix_timestamp, pair.second);
        }

        std::string serialized_message_list = app_market_data_outbox_message_list.SerializeAsString();
        redis_connector.append_publish_command(market_data_publish_channel.c_str(), serialized_message_list.c_str());

        int command_count = latest_data_map.size() + 1;

        bool publish_success = false;

        for (size_t i = 0; i < command_count; ++i)
        {
            redisReply *reply = redis_connector.get_reply();
            if (i == command_count - 1)
            {
                publish_success = reply && reply->type == REDIS_REPLY_INTEGER && reply->integer > 0;
            }

            redis_connector.free_reply(reply);
        }

        if (publish_success)
        {
            work.exec_params(purge_outbox_messages_query, pqxx::params{ids});
            work.commit();
        }
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}