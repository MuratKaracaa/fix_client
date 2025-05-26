#pragma once
#include "concurrentqueue.h"
#include "rdkafka.h"
#include <atomic>
#include <thread>
#include "app_market_data.h"
#include "BS_thread_pool.hpp"
#include "pqxx/pqxx"
#include "constants.h"

class MarketDataConsumer
{
public:
    MarketDataConsumer(moodycamel::ConcurrentQueue<AppMarketData> &market_data_queue, size_t batch_size, size_t thread_count);

    void start();
    void stop();

private:
    moodycamel::ConcurrentQueue<AppMarketData> &market_data_queue;
    std::unique_ptr<BS::thread_pool> thread_pool;

    size_t batch_size;
    size_t thread_count;

    std::vector<moodycamel::ConsumerToken> consumer_tokens;
    std::vector<pqxx::connection> connections;

    void process_messages(std::vector<AppMarketData> &app_market_data, pqxx::connection &connection, size_t count);
    std::string prepare_update_stock_and_create_outbox_query(pqxx::work &work, const std::unordered_map<std::string, std::pair<std::string, double>> &latest_price_map);
};