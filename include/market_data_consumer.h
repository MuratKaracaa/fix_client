#include <concurrentqueue.h>
#include <rdkafka.h>
#include <atomic>
#include <thread>
#include "app_market_data.h"
#include <BS_thread_pool.hpp>
#include <pqxx/pqxx>

class MarketDataConsumer
{
public:
    MarketDataConsumer(moodycamel::ConcurrentQueue<AppMarketData> &market_data_queue, size_t batch_size, size_t thread_count);

    void start();
    void stop();

private:
    std::unique_ptr<BS::thread_pool> thread_pool;

    size_t batch_size;
    size_t thread_count;

    std::vector<moodycamel::ConsumerToken> consumer_tokens;
    std::vector<pqxx::connection> connections;
};