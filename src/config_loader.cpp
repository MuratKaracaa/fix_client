#include "config_loader.h"
#include <fstream>
#include <sstream>

template <typename T>
T ConfigLoader::get_env_or_default(const std::string &env_var, const T &default_value)
{
    const char *value = std::getenv(env_var.c_str());
    return value ? T(value) : default_value;
}

std::string ConfigLoader::get_env_required(const std::string &env_var)
{
    const char *value = std::getenv(env_var.c_str());
    if (!value)
    {
        throw std::runtime_error("Environment variable " + env_var + " is not set");
    }
    return value;
}