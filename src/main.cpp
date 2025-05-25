#include <concurrentqueue.h>
#include <execution_report_consumer.h>
#include <constants.h>

int main()
{
    moodycamel::ConcurrentQueue<std::string> execution_report_queue;
    moodycamel::ConcurrentQueue<std::string> market_data_update_queue;

    ExecutionReportConsumer execution_report_consumer(execution_report_queue, execution_report_thread_count, kafka_topic_name, execution_report_batch_size);

    global_execution_report_consumer_running.store(true, std::memory_order_relaxed);
    execution_report_consumer.start();

    std::this_thread::sleep_for(std::chrono::seconds(10));
    global_execution_report_consumer_running.store(false, std::memory_order_relaxed);
    execution_report_consumer.stop();

    return 0;
}