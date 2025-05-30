#include "redis_connector.h"
#include "constants.h"

RedisConnector::RedisConnector()
{
    conn = redisConnect(redis_hostname.c_str(), redis_port);
}

RedisConnector::~RedisConnector()
{
    redisFree(conn);
}

bool RedisConnector::publishMessage(const std::string &message)
{
    redisReply *reply = static_cast<redisReply *>(redisCommand(conn, "PUBLISH %s %s", market_data_publish_channel.c_str(), message.c_str()));
    if (reply == nullptr)
    {
        return false;
    }
    bool result = reply->type == REDIS_REPLY_INTEGER && reply->integer > 0;
    freeReplyObject(reply);
    return result;
}
