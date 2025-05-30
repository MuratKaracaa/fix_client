#pragma once

#include <thread>
#include "pqxx/pqxx"
#include "redis_connector.h"

class MarketDataOutboxRelay
{
private:
    std::thread worker_thread;
    pqxx::connection connection;
    RedisConnector &redis_connector;

    void process_outbox_messages();

public:
    MarketDataOutboxRelay(RedisConnector &redis_connector);

    void start();
    void stop();
};