#include <concurrentqueue.h>
#include <execution_report_consumer.h>
#include <constants.h>
#include <thread>
#include <chrono>
#include <app_execution_report.pb.h>
#include <app_market_data.h>

int main()
{
    moodycamel::ConcurrentQueue<AppExecutionReport> execution_report_queue;
    moodycamel::ConcurrentQueue<AppMarketData> market_data_update_queue;

    ExecutionReportConsumer execution_report_consumer(execution_report_queue, kafka_topic_name);

    execution_report_consumer.start();

    std::this_thread::sleep_for(std::chrono::seconds(10));
    execution_report_consumer.stop();

    return 0;
}