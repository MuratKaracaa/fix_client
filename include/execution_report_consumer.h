#include <concurrentqueue.h>
#include <rdkafka.h>
#include <atomic>
#include <thread>

class ExecutionReportConsumer
{
public:
    ExecutionReportConsumer(moodycamel::ConcurrentQueue<AppExecutionReport> &execution_report_queue, const std::string &topic_name);
    ~ExecutionReportConsumer();

    void start();
    void stop();

private:
    moodycamel::ConcurrentQueue<AppExecutionReport> &execution_report_queue_;
    std::thread worker_thread;
    moodycamel::ConsumerToken consumer_token;

    rd_kafka_t *producer;
    rd_kafka_topic_t *topic;

    void process_messages();
};