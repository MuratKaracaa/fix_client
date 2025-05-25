#include "execution_report_consumer.h"
#include <constants.h>
#include <config_loader.h>
#include <iostream>
#include <chrono>
#include <thread>

ExecutionReportConsumer::ExecutionReportConsumer(moodycamel::ConcurrentQueue<std::string> &execution_report_queue, size_t thread_count, const std::string &topic_name, size_t batch_size)
    : execution_report_queue_(execution_report_queue), thread_count(thread_count), batch_size(batch_size)
{

    thread_pool = std::make_unique<BS::thread_pool>(thread_count);
    consumer_tokens.reserve(thread_count);

    for (size_t i = 0; i < thread_count; ++i)
    {
        consumer_tokens.emplace_back(execution_report_queue_);
    }
    char errstr[512];
    std::string max_in_flight_calculated = std::to_string(thread_count * max_in_flight_requests_per_connection);
    rd_kafka_conf_t *conf = rd_kafka_conf_new();
    if (rd_kafka_conf_set(conf, "bootstrap.servers", ConfigLoader::get_env_required("KAFKA_BROKERS").c_str(), errstr, sizeof(errstr)) != RD_KAFKA_CONF_OK)
    {
        rd_kafka_conf_destroy(conf);
        throw std::runtime_error("Failed to set bootstrap.servers: " + std::string(errstr));
    }
    rd_kafka_conf_set(conf, "acks", kafka_acks.c_str(), errstr, sizeof(errstr));
    rd_kafka_conf_set(conf, "retries", kafka_retries.c_str(), errstr, sizeof(errstr));
    rd_kafka_conf_set(conf, "retry.backoff.ms", kafka_retry_backoff_ms.c_str(), errstr, sizeof(errstr));
    rd_kafka_conf_set(conf, "delivery.timeout.ms", kafka_delivery_timeout_ms.c_str(), errstr, sizeof(errstr));
    rd_kafka_conf_set(conf, "enable.idempotence", kafka_idempotence.c_str(), errstr, sizeof(errstr));
    rd_kafka_conf_set(conf, "max.in.flight.requests.per.connection", max_in_flight_calculated.c_str(), errstr, sizeof(errstr));
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
    if (producer)
    {
        rd_kafka_destroy(producer);
    }
}

void ExecutionReportConsumer::start()
{
    for (size_t i = 0; i < thread_count; ++i)
    {
        thread_pool->submit_task([this, i]()
                                 {
            while (global_execution_report_consumer_running.load(std::memory_order_relaxed))
            {

            } });
    }
}

void ExecutionReportConsumer::stop()
{
    global_execution_report_consumer_running.store(false, std::memory_order_relaxed);
    if (thread_pool)
    {
        thread_pool->wait();
    }
}