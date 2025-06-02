#include "redis_connector.h"
#include "constants.h"
#include "app_market_data_outbox_message.pb.h"
#include "utils.h"

RedisConnector::RedisConnector()
{
    conn = redisConnect(redis_hostname.c_str(), redis_port);
}

RedisConnector::~RedisConnector()
{
    redisFree(conn);
}

bool RedisConnector::append_publish_command(const std::string &channel, const std::string &message)
{
    return redisAppendCommand(conn, "PUBLISH %s %s", channel.c_str(), message.c_str());
}

bool RedisConnector::append_timeseries_command(const std::string &key, int64_t timestamp, double value)
{
    return redisAppendCommand(conn, "TS.ADD %s %lld %f", key.c_str(), timestamp, value);
}

redisReply *RedisConnector::get_reply()
{
    redisReply *reply = nullptr;
    redisGetReply(conn, (void **)&reply);
    return reply;
}

void RedisConnector::free_reply(redisReply *reply)
{
    freeReplyObject(reply);
}