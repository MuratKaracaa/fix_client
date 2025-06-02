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

    /**
     * @brief Append a publish command to the Redis pipeline.
     * @param channel The channel to publish the message to.
     * @param message The message to publish.
     * @return True if the command was appended successfully, false otherwise.
     */
    bool append_publish_command(const std::string &channel, const std::string &message);

    /**
     * @brief Append a timeseries command to the Redis pipeline.
     * @param key The key to add the timeseries data to.
     * @param timestamp The timestamp of the data.
     * @param value The value of the data.
     * @return True if the command was appended successfully, false otherwise.
     */
    bool append_timeseries_command(const std::string &key, int64_t timestamp, double value);

    /**
     * @brief Get a reply from the Redis pipeline. Triggers network call if called after appending commands.
     * @return A pointer to the reply. nullptr if there is no reply or on error.
     */
    redisReply *get_reply();

    /**
     * @brief Free a reply from memory.
     * @param reply The reply to free.
     */
    void free_reply(redisReply *reply);
};