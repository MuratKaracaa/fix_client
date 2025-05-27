#include "rabbit_mq_connector.h"

RabbitMQConnector::RabbitMQConnector()
{
    disconnect();
}

bool RabbitMQConnector::connect(const std::string &hostname, int port,
                                const std::string &username, const std::string &password)
{
    conn = amqp_new_connection();
    socket = amqp_tcp_socket_new(conn);

    if (!socket)
    {
        std::cerr << "Creating TCP socket failed" << std::endl;
        return false;
    }

    int status = amqp_socket_open(socket, hostname.c_str(), port);
    if (status)
    {
        std::cerr << "Opening TCP socket failed" << std::endl;
        return false;
    }

    amqp_rpc_reply_t reply = amqp_login(conn, "/", 0, 131072, 0,
                                        AMQP_SASL_METHOD_PLAIN,
                                        username.c_str(), password.c_str());
    if (reply.reply_type != AMQP_RESPONSE_NORMAL)
    {
        std::cerr << "Login failed" << std::endl;
        return false;
    }

    amqp_channel_open(conn, 1);
    reply = amqp_get_rpc_reply(conn);
    if (reply.reply_type != AMQP_RESPONSE_NORMAL)
    {
        std::cerr << "Opening channel failed" << std::endl;
        return false;
    }

    return true;
}

bool RabbitMQConnector::declareQueue(const std::string &queueName)
{
    amqp_queue_declare_ok_t *r = amqp_queue_declare(
        conn, 1, amqp_cstring_bytes(queueName.c_str()),
        0, 1, 0, 0, amqp_empty_table);

    amqp_rpc_reply_t reply = amqp_get_rpc_reply(conn);
    return reply.reply_type == AMQP_RESPONSE_NORMAL;
}

bool RabbitMQConnector::publishMessage(const std::string &queueName, const std::string &message)
{
    amqp_basic_properties_t props;
    props._flags = AMQP_BASIC_CONTENT_TYPE_FLAG | AMQP_BASIC_DELIVERY_MODE_FLAG;
    props.content_type = amqp_cstring_bytes("text/plain");
    props.delivery_mode = 2;

    int result = amqp_basic_publish(
        conn,
        1,
        amqp_cstring_bytes(""),
        amqp_cstring_bytes(queueName.c_str()),
        0,
        0,
        &props,
        amqp_cstring_bytes(message.c_str()));

    return result == AMQP_STATUS_OK;
}

void RabbitMQConnector::disconnect()
{
    amqp_channel_close(conn, 1, AMQP_REPLY_SUCCESS);
    amqp_connection_close(conn, AMQP_REPLY_SUCCESS);
    amqp_destroy_connection(conn);
}