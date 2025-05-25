#include <atomic>
#include <string>
#include "config_loader.h"

inline std::atomic<bool> global_execution_report_consumer_running{false};

const size_t max_in_flight_requests_per_connection = ConfigLoader::get_env_or_default<size_t>("KAFKA_MAX_IN_FLIGHT_REQUESTS_PER_CONNECTION", 5);
const size_t execution_report_batch_size = ConfigLoader::get_env_or_default<size_t>("KAFKA_BATCH_SIZE", 262144);
const size_t execution_report_thread_count = ConfigLoader::get_env_or_default<size_t>("KAFKA_THREAD_COUNT", 8);
const std::string kafka_acks = ConfigLoader::get_env_or_default<std::string>("KAFKA_ACKS", "all");
const std::string kafka_retries = ConfigLoader::get_env_or_default<std::string>("KAFKA_RETRIES", "2147483647");
const std::string kafka_retry_backoff_ms = "100";
const std::string kafka_delivery_timeout_ms = "300000";
const std::string kafka_idempotence = ConfigLoader::get_env_or_default<std::string>("KAFKA_IDEMPOTENCE", "true");
const std::string kafka_linger_ms = ConfigLoader::get_env_or_default<std::string>("KAFKA_LINGER_MS", "1");
const std::string kafka_compression = ConfigLoader::get_env_or_default<std::string>("KAFKA_COMPRESSION", "lz4");
const std::string kafka_queue_buffering_max_messages = ConfigLoader::get_env_or_default<std::string>("KAFKA_QUEUE_BUFFERING_MAX_MESSAGES", "10000000");
const std::string kafka_queue_buffering_max_kbytes = ConfigLoader::get_env_or_default<std::string>("KAFKA_QUEUE_BUFFERING_MAX_KBYTES", "4194304");
const std::string kafka_topic_name = ConfigLoader::get_env_or_default<std::string>("KAFKA_EXECUTION_REPORT_TOPIC_NAME", "execution_report");