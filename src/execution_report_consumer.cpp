#include "execution_report_consumer.h"
#include <constants.h>
#include "app_config_loader.h"
#include <iostream>
#include <chrono>
#include <thread>
#include "test_message.pb.h"
#include <random>
#include <string>

ExecutionReportConsumer::ExecutionReportConsumer(std::queue<std::string> &execution_report_queue, const std::string &topic_name)
    : execution_report_queue_(execution_report_queue)
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
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> id_dist(1, 10000);
    std::uniform_real_distribution<double> balance_dist(1.0, 10000.0);

    const std::vector<std::string> sample_names = {
        "Alpha", "Beta", "Gamma", "Delta", "Epsilon",
        "Zeta", "Eta", "Theta", "Iota", "Kappa",
        "Lambda", "Mu", "Nu", "Xi", "Omicron",
        "Pi", "Rho", "Sigma", "Tau", "Upsilon",
        "Phi", "Chi", "Psi", "Omega"};
    std::uniform_int_distribution<size_t> name_dist(0, sample_names.size() - 1);

    while (global_execution_report_consumer_running.load(std::memory_order_relaxed))
    {
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
}