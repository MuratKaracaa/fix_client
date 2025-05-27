#pragma once
#include <unordered_map>
#include <string>
#include <cstdlib>

class AppConfigLoader
{
public:
    static int get_env_or_default(const std::string &env_var, const size_t &default_value);
    static std::string get_env_or_default(const std::string &env_var, const std::string &default_value);
    static std::string get_env_required(const std::string &env_var);
};