#pragma once
#include <iostream>
#include <string>
#include <cstring>
#include "hiredis.h"

/**
 * @brief RedisConnector is a wrapper class for the hiredis library.
 * It provides a simple interface for appending commands to a Redis pipeline and getting replies.
 * It also provides a simple interface for freeing replies.
 *
 * @note This class is not thread-safe. Append commands to a Redis pipeline and get replies in a single thread.
 */
class RedisConnector
{
private:
    redisContext *conn;

public:
    RedisConnector();
    ~RedisConnector() noexcept;

    bool append_publish_command(const std::string &channel, const std::string &message);
    bool append_timeseries_command(const std::string &key, int64_t timestamp, double value);

    redisReply *get_reply();
    void free_reply(redisReply *reply);
};