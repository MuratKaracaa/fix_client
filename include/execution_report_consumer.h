#pragma once
#include "concurrentqueue.h"
#include <atomic>
#include <thread>
#include "app_execution_report.pb.h"
#include "kafka_connector.h"

class ExecutionReportConsumer
{
public:
    ExecutionReportConsumer(moodycamel::ConcurrentQueue<AppExecutionReport> &execution_report_queue, KafkaConnector &kafka_connector);
    ~ExecutionReportConsumer();

    void start();
    void stop();

private:
    moodycamel::ConcurrentQueue<AppExecutionReport> &execution_report_queue_;
    KafkaConnector &kafka_connector;
    std::thread worker_thread;
    moodycamel::ConsumerToken consumer_token;

    void process_messages();
};