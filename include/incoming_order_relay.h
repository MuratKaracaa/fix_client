#pragma once

#include "pqxx/pqxx"
#include "Session.h"
#include <thread>

class IncomingOrderRelay
{
private:
    pqxx::connection connection;
    std::thread worker_thread;
    FIX::SessionID &session_id;
    void process_incoming_orders();

public:
    IncomingOrderRelay(FIX::SessionID &session_id);

    void start();
    void stop();
};