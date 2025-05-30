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

    bool publishMessage(const std::string &message);
};