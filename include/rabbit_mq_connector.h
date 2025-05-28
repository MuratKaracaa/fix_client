#pragma once
#include <iostream>
#include <string>
#include <cstring>

extern "C"
{
#include <amqp.h>
#include <amqp_tcp_socket.h>
}

class RabbitMQConnector
{
private:
    amqp_connection_state_t conn;
    amqp_socket_t *socket;

public:
    ~RabbitMQConnector() noexcept;

    bool connect();

    bool setupFanoutExchange(const std::string &exchangeName);

    bool publishMessage(const std::string &message);

    void disconnect();
};