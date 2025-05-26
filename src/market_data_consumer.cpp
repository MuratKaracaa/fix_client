#include "market_data_consumer.h"
#include <app_config_loader.h>

MarketDataConsumer::MarketDataConsumer(moodycamel::ConcurrentQueue<AppMarketData> &market_data_queue, size_t batch_size, size_t thread_count)
    : thread_pool(std::make_unique<BS::thread_pool>(thread_count)),
      batch_size(batch_size),
      thread_count(thread_count)
{
    consumer_tokens.reserve(thread_count);
    connections.reserve(thread_count);

    for (size_t i = 0; i < thread_count; ++i)
    {
        consumer_tokens.emplace_back(market_data_queue);
        connections.push_back(pqxx::connection(AppConfigLoader::get_env_required("DATABASE_CONNECTION_STRING")));
    }

    bool is_all_connections_open = std::all_of(connections.begin(), connections.end(), [](const pqxx::connection &conn)
                                               { return conn.is_open(); });
    if (!is_all_connections_open)
    {
        throw std::runtime_error("Failed to connect to database");
    }
}