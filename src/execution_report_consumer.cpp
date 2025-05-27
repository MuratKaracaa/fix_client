#include "execution_report_consumer.h"
#include "constants.h"
#include "app_config_loader.h"
#include <iostream>
#include <chrono>
#include <thread>
#include <random>
#include <string>
#include "app_execution_report.pb.h"

ExecutionReportConsumer::ExecutionReportConsumer(moodycamel::ConcurrentQueue<AppExecutionReport> &execution_report_queue, KafkaConnector &kafka_connector)
    : execution_report_queue_(execution_report_queue),
      kafka_connector(kafka_connector),
      consumer_token(execution_report_queue)
{
}

ExecutionReportConsumer::~ExecutionReportConsumer()
{
    stop();
}

void ExecutionReportConsumer::process_messages()
{
    while (global_execution_report_consumer_running.load(std::memory_order_relaxed))
    {
        std::vector<AppExecutionReport> app_execution_reports(execution_report_consuming_batch_size);
        size_t size = execution_report_queue_.try_dequeue_bulk(consumer_token, app_execution_reports.data(), execution_report_consuming_batch_size);
        if (size > 0)
        {
            for (size_t i = 0; i < size; ++i)
            {
                AppExecutionReport app_execution_report = std::move(app_execution_reports[i]);
                std::string serialized_app_execution_report;
                app_execution_report.SerializeToString(&serialized_app_execution_report);
                kafka_connector.publish_message(serialized_app_execution_report);
            }
        }
        else
        {
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
        kafka_connector.poll(0);
    }
}

void ExecutionReportConsumer::start()
{
    global_execution_report_consumer_running.store(true, std::memory_order_relaxed);
    worker_thread = std::thread(&ExecutionReportConsumer::process_messages, this);
}

void ExecutionReportConsumer::stop()
{
    global_execution_report_consumer_running.store(false, std::memory_order_relaxed);
    if (worker_thread.joinable())
    {
        worker_thread.join();
    }

    AppExecutionReport app_execution_report;
    while (execution_report_queue_.try_dequeue(app_execution_report))
    {
        std::string serialized_app_execution_report;
        app_execution_report.SerializeToString(&serialized_app_execution_report);
        kafka_connector.publish_message(serialized_app_execution_report);
    }
    kafka_connector.poll(0);
}