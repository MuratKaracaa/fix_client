#include <concurrentqueue.h>
#include <rdkafka.h>
#include <atomic>
#include <thread>

class ExecutionReportConsumer
{
public:
    ExecutionReportConsumer(std::queue<std::string> &execution_report_queue, const std::string &topic_name);
    ~ExecutionReportConsumer();

    void start();
    void stop();

private:
    std::queue<std::string> &execution_report_queue_;
    std::thread worker_thread;

    rd_kafka_t *producer;
    rd_kafka_topic_t *topic;

    void process_messages();
};