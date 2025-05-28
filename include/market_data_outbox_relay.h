
#include <thread>
#include "pqxx/pqxx"
#include "rabbit_mq_connector.h"

class MarketDataOutboxRelay
{
private:
    std::thread worker_thread;
    pqxx::connection connection;
    RabbitMQConnector &rabbitmq_connector;

    void process_outbox_messages();

public:
    MarketDataOutboxRelay(RabbitMQConnector &rabbitmq_connector);

    void start();
    void stop();
};