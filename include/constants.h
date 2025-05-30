#pragma once

#include <atomic>
#include <string>
#include "app_config_loader.h"

inline std::atomic<bool> global_execution_report_consumer_running{false};
inline std::atomic<bool> global_market_data_consumer_running{false};
inline std::atomic<bool> global_outbox_relay_running{false};
inline std::atomic<bool> global_incoming_order_relay_running{false};

const int max_in_flight_requests_per_connection = AppConfigLoader::get_env_or_default("KAFKA_MAX_IN_FLIGHT_REQUESTS_PER_CONNECTION", 5);
const int execution_report_batch_size = AppConfigLoader::get_env_or_default("KAFKA_BATCH_SIZE", 262144);
const int execution_report_consuming_batch_size = AppConfigLoader::get_env_or_default("KAFKA_EXECUTION_REPORT_CONSUMING_BATCH_SIZE", 10000);
const std::string kafka_acks = AppConfigLoader::get_env_or_default("KAFKA_ACKS", "all");
const std::string kafka_retries = AppConfigLoader::get_env_or_default("KAFKA_RETRIES", "2147483647");
const std::string kafka_retry_backoff_ms = "100";
const std::string kafka_delivery_timeout_ms = "300000";
const std::string kafka_idempotence = AppConfigLoader::get_env_or_default("KAFKA_IDEMPOTENCE", "true");
const std::string kafka_linger_ms = AppConfigLoader::get_env_or_default("KAFKA_LINGER_MS", "1");
const std::string kafka_compression = AppConfigLoader::get_env_or_default("KAFKA_COMPRESSION", "lz4");
const std::string kafka_queue_buffering_max_messages = AppConfigLoader::get_env_or_default("KAFKA_QUEUE_BUFFERING_MAX_MESSAGES", "10000000");
const std::string kafka_queue_buffering_max_kbytes = AppConfigLoader::get_env_or_default("KAFKA_QUEUE_BUFFERING_MAX_KBYTES", "4194304");
const std::string kafka_topic_name = AppConfigLoader::get_env_or_default("KAFKA_EXECUTION_REPORT_TOPIC_NAME", "execution_report");

const std::string redis_hostname = AppConfigLoader::get_env_required("REDIS_HOST");
const int redis_port = std::stoi(AppConfigLoader::get_env_required("REDIS_PORT"));
const std::string market_data_publish_channel = AppConfigLoader::get_env_or_default("REDIS_CHANNEL", "market_data_publish_channel");

const std::string database_connection_string = AppConfigLoader::get_env_required("DATABASE_CONNECTION_STRING");

const std::string fetch_outbox_messages_query = "SELECT * FROM stock_update_outbox";
const std::string fetch_incoming_orders_query = "SELECT * FROM incoming_orders";
const std::string purge_outbox_messages_query = "DELETE FROM stock_update_outbox where id = ANY($1)";
const std::string purge_incoming_orders_query = "DELETE FROM incoming_orders where order_id = ANY($1)";
