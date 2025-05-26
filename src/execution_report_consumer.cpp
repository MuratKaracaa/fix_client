#include "execution_report_consumer.h"
#include "constants.h"
#include "app_config_loader.h"
#include <iostream>
#include <chrono>
#include <thread>
#include <random>
#include <string>
#include "app_execution_report.pb.h"

ExecutionReportConsumer::ExecutionReportConsumer(moodycamel::ConcurrentQueue<AppExecutionReport> &execution_report_queue, const std::string &topic_name)
    : execution_report_queue_(execution_report_queue),
      consumer_token(execution_report_queue)
{
    char errstr[512];
    rd_kafka_conf_t *conf = rd_kafka_conf_new();
    if (rd_kafka_conf_set(conf, "bootstrap.servers", AppConfigLoader::get_env_required("KAFKA_BROKERS").c_str(), errstr, sizeof(errstr)) != RD_KAFKA_CONF_OK)
    {
        rd_kafka_conf_destroy(conf);
        throw std::runtime_error("Failed to set bootstrap.servers: " + std::string(errstr));
    }
    rd_kafka_conf_set(conf, "acks", kafka_acks.c_str(), errstr, sizeof(errstr));
    rd_kafka_conf_set(conf, "retries", kafka_retries.c_str(), errstr, sizeof(errstr));
    rd_kafka_conf_set(conf, "retry.backoff.ms", kafka_retry_backoff_ms.c_str(), errstr, sizeof(errstr));
    rd_kafka_conf_set(conf, "delivery.timeout.ms", kafka_delivery_timeout_ms.c_str(), errstr, sizeof(errstr));
    rd_kafka_conf_set(conf, "enable.idempotence", kafka_idempotence.c_str(), errstr, sizeof(errstr));
    rd_kafka_conf_set(conf, "batch.size", std::to_string(execution_report_batch_size).c_str(), errstr, sizeof(errstr));
    rd_kafka_conf_set(conf, "linger.ms", kafka_linger_ms.c_str(), errstr, sizeof(errstr));
    rd_kafka_conf_set(conf, "compression.type", kafka_compression.c_str(), errstr, sizeof(errstr));
    rd_kafka_conf_set(conf, "queue.buffering.max.messages", kafka_queue_buffering_max_messages.c_str(), errstr, sizeof(errstr));
    rd_kafka_conf_set(conf, "queue.buffering.max.kbytes", kafka_queue_buffering_max_kbytes.c_str(), errstr, sizeof(errstr));

    producer = rd_kafka_new(RD_KAFKA_PRODUCER, conf, errstr, sizeof(errstr));
    if (!producer)
    {
        rd_kafka_conf_destroy(conf);
        throw std::runtime_error("Failed to create producer: " + std::string(errstr));
    }

    topic = rd_kafka_topic_new(producer, topic_name.c_str(), NULL);
    if (!topic)
    {
        rd_kafka_destroy(producer);
        throw std::runtime_error("Failed to create topic: " + std::string(errstr));
    }
}

ExecutionReportConsumer::~ExecutionReportConsumer()
{
    stop();
    if (topic)
    {
        rd_kafka_topic_destroy(topic);
    }
    if (producer)
    {
        rd_kafka_destroy(producer);
    }
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
                publish_to_kafka(serialized_app_execution_report);
            }
        }
        else
        {
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
        rd_kafka_poll(producer, 0);
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
        publish_to_kafka(serialized_app_execution_report);
    }
    rd_kafka_poll(producer, 0);
}

void ExecutionReportConsumer::publish_to_kafka(std::string &app_execution_report_serialized)
{
    rd_kafka_produce(topic, RD_KAFKA_PARTITION_UA, RD_KAFKA_MSG_F_COPY,
                     static_cast<void *>(const_cast<char *>(app_execution_report_serialized.c_str())),
                     app_execution_report_serialized.size(), NULL, 0, NULL);
}