#include "redis_connector.h"
#include "constants.h"
#include "app_market_data_outbox_message.pb.h"
#include "utils.h"

RedisConnector::RedisConnector()
{
    conn = redisConnect(redis_hostname.c_str(), redis_port);
}

RedisConnector::~RedisConnector()
{
    redisFree(conn);
}

bool RedisConnector::publish_message_and_add_timeseries_data(std::unordered_map<std::string, std::pair<std::string, double>> &latest_data_map)
{
    AppMarketDataOutboxMessageList app_market_data_outbox_message_list;
    auto *messages = app_market_data_outbox_message_list.mutable_messages();
    messages->Reserve(latest_data_map.size());

    for (const auto &[stock_symbol, pair] : latest_data_map)
    {
        AppMarketDataOutboxMessage app_market_data_outbox_message;
        app_market_data_outbox_message.set_stock_symbol(stock_symbol);
        app_market_data_outbox_message.set_latest_price(pair.second);
        app_market_data_outbox_message.set_timestamp(pair.first);
        messages->Add(std::move(app_market_data_outbox_message));

        int64_t unix_timestamp = Utils::convert_iso_timestamp_to_unix_timestamp(pair.first);
        redisAppendCommand(conn, "TS.ADD %s %lld %f", stock_symbol.c_str(), unix_timestamp, pair.second);
    }

    std::string serialized_message_list = app_market_data_outbox_message_list.SerializeAsString();
    redisAppendCommand(conn, "PUBLISH %s %s", market_data_publish_channel.c_str(), serialized_message_list.c_str());

    int command_count = latest_data_map.size() + 1;

    bool publish_success = false;

    for (size_t i = 0; i < command_count; ++i)
    {
        redisReply *reply = nullptr;
        redisGetReply(conn, (void **)&reply);
        if (i == command_count - 1)
        {
            publish_success = reply && reply->type == REDIS_REPLY_INTEGER && reply->integer > 0;
        }

        freeReplyObject(reply);
    }

    return publish_success;
}
