#include <unordered_map>
#include <string>

class ConfigLoader
{
public:
    template <typename T>
    static T get_env_or_default(const std::string &env_var, const T &default_value);
    static std::string get_env_required(const std::string &env_var);
};