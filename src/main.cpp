#include "concurrentqueue.h"
#include "execution_report_consumer.h"
#include "constants.h"
#include <thread>
#include <chrono>
#include "app_execution_report.pb.h"
#include "app_market_data.h"
#include "market_data_consumer.h"
#include "Session.h"
#include <iostream>
#include "order_side.h"
#include "client_application.h"
#include "incoming_order_relay.h"
#include "rabbit_mq_connector.h"
#include "outbox_relay.h"
#include "FileStore.h"
#include "FileLog.h"
#include "SocketInitiator.h"

int main()
{
    moodycamel::ConcurrentQueue<AppExecutionReport> execution_report_queue;
    moodycamel::ConcurrentQueue<AppMarketData> market_data_update_queue;
    FIX::SessionID session_id;

    try
    {
        KafkaConnector kafka_connector;
        RabbitMQConnector rabbitmq_connector;
        ExecutionReportConsumer execution_report_consumer(execution_report_queue, kafka_connector);
        MarketDataConsumer market_data_consumer(market_data_update_queue, 8, 1024);
        IncomingOrderRelay incoming_order_relay(session_id);
        OutboxRelay outbox_relay(rabbitmq_connector);

        std::string config_file = "fix_client.cfg";
        FIX::SessionSettings session_settings(config_file);
        ClientApplication client_application(execution_report_queue, market_data_update_queue, session_id);
        FIX::FileStoreFactory store_factory(session_settings);
        FIX::FileLogFactory log_factory(session_settings);
        FIX::SocketInitiator initiator(client_application, store_factory, session_settings, log_factory);

        execution_report_consumer.start();
        market_data_consumer.start();
        incoming_order_relay.start();
        outbox_relay.start();

        initiator.start();

        // TODO: Add a signal handler to stop the application al together or turning booleans to false and stopping individual components

        execution_report_consumer.stop();
        market_data_consumer.stop();
        incoming_order_relay.stop();
        outbox_relay.stop();
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
        return 1;
    }

    return 0;
}