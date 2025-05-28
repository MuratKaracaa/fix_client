#include "kafka_connector.h"
#include "constants.h"
#include "app_config_loader.h"
#include <stdexcept>

KafkaConnector::KafkaConnector()
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

    topic = rd_kafka_topic_new(producer, kafka_topic_name.c_str(), NULL);
    if (!topic)
    {
        rd_kafka_destroy(producer);
        throw std::runtime_error("Failed to create topic: " + std::string(errstr));
    }
}

KafkaConnector::~KafkaConnector()
{
    if (topic)
    {
        rd_kafka_topic_destroy(topic);
    }
    if (producer)
    {
        rd_kafka_destroy(producer);
    }
}

void KafkaConnector::publish_message(std::string &serialized_message)
{
    rd_kafka_produce(topic, RD_KAFKA_PARTITION_UA, RD_KAFKA_MSG_F_COPY,
                     static_cast<void *>(const_cast<char *>(serialized_message.c_str())),
                     serialized_message.size(), NULL, 0, NULL);
}

void KafkaConnector::poll(int timeout_ms)
{
    rd_kafka_poll(producer, timeout_ms);
}