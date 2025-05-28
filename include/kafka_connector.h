#pragma once
#include "rdkafka.h"
#include <string>

class KafkaConnector
{
public:
    KafkaConnector();
    ~KafkaConnector();

    void publish_message(std::string &serialized_message);
    void poll(int timeout_ms);

private:
    rd_kafka_t *producer;
    rd_kafka_topic_t *topic;
};