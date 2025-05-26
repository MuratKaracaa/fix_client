#include "app_config_loader.h"
#include <fstream>
#include <sstream>

size_t AppConfigLoader::get_env_or_default(const std::string &env_var, const size_t &default_value)
{
    const char *value = std::getenv(env_var.c_str());
    if (value)
    {
        std::istringstream iss(value);
        size_t result;
        if (iss >> result)
        {
            return result;
        }
    }
    return default_value;
}

std::string AppConfigLoader::get_env_or_default(const std::string &env_var, const std::string &default_value)
{
    const char *value = std::getenv(env_var.c_str());
    return value ? std::string(value) : default_value;
}

std::string AppConfigLoader::get_env_required(const std::string &env_var)
{
    const char *value = std::getenv(env_var.c_str());
    if (!value)
    {
        throw std::runtime_error("Environment variable " + env_var + " is not set");
    }
    return value;
}