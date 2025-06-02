#pragma once
#include <iostream>
#include <string>
#include <cstring>
#include "hiredis.h"

class RedisConnector
{
private:
    redisContext *conn;

public:
    RedisConnector();
    ~RedisConnector() noexcept;

    bool publish_message_and_add_timeseries_data(std::unordered_map<std::string, std::pair<std::string, double>> &latest_data_map);
};