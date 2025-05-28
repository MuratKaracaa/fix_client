#include "market_data_outbox_relay.h"
#include "constants.h"
#include "app_market_data_outbox_message.pb.h"

MarketDataOutboxRelay::MarketDataOutboxRelay(RabbitMQConnector &rabbitmq_connector)
    : rabbitmq_connector(rabbitmq_connector)
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
        AppMarketDataOutboxMessageList app_market_data_outbox_message_list;
        auto *messages = app_market_data_outbox_message_list.mutable_messages();
        messages->Reserve(result.size());
        for (const auto &row : result)
        {
            AppMarketDataOutboxMessage app_market_data_outbox_message;
            app_market_data_outbox_message.set_stock_symbol(row["stock_symbol"].as<std::string>());
            app_market_data_outbox_message.set_latest_price(row["latest_trading_price"].as<double>());
            app_market_data_outbox_message.set_timestamp(row["latest_trading_price_time_stamp"].as<std::string>());
            messages->Add(std::move(app_market_data_outbox_message));
        }
        std::string serialized_message_list = app_market_data_outbox_message_list.SerializeAsString();
        bool publish_result = rabbitmq_connector.publishMessage(serialized_message_list);
        if (publish_result)
        {
            work.exec(purge_outbox_messages_query);
            work.commit();
        }
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}