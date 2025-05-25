#include <concurrentqueue.h>
#include <BS_thread_pool.hpp>
#include <rdkafka.h>

class ExecutionReportConsumer
{
public:
    ExecutionReportConsumer(moodycamel::ConcurrentQueue<std::string> &execution_report_queue, size_t thread_count, const std::string &topic_name, size_t batch_size);
    ~ExecutionReportConsumer();

    void start();
    void stop();

private:
    moodycamel::ConcurrentQueue<std::string> &execution_report_queue_;
    std::unique_ptr<BS::thread_pool> thread_pool;
    size_t thread_count;
    size_t batch_size;

    rd_kafka_t *producer;
    rd_kafka_topic_t *topic;
    std::vector<moodycamel::ConsumerToken> consumer_tokens;
};