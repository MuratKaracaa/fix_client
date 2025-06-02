#include "incoming_order_relay.h"
#include "constants.h"
#include "order_side.h"
#include "order_type.h"
#include "NewOrderSingle.h"

IncomingOrderRelay::IncomingOrderRelay(FIX::SessionID &session_id)
    : session_id(session_id)
{
    connection = pqxx::connection(database_connection_string);
}

void IncomingOrderRelay::start()
{
    global_incoming_order_relay_running.store(true, std::memory_order_relaxed);

    worker_thread = std::thread(&IncomingOrderRelay::process_incoming_orders, this);
}

void IncomingOrderRelay::stop()
{
    global_incoming_order_relay_running.store(false, std::memory_order_relaxed);
    worker_thread.join();
}

void IncomingOrderRelay::process_incoming_orders()
{
    while (global_incoming_order_relay_running.load(std::memory_order_relaxed))
    {
        try
        {
            pqxx::work work(connection);
            pqxx::result result = work.exec(fetch_incoming_orders_outbox_query);
            std::vector<std::string> ids;
            ids.reserve(result.size());
            for (const auto &row : result)
            {
                std::string order_id = row["order_id"].as<std::string>();
                ids.push_back(std::move(order_id));
                std::string symbol = row["symbol"].as<std::string>();
                OrderType type = static_cast<OrderType>(row["type"].as<int>());
                OrderSide side = static_cast<OrderSide>(row["side"].as<int>());
                std::string quantity = row["quantity"].as<std::string>();
                std::string price = row["price"].as<std::string>();
                std::string timestamp = row["timestamp"].as<std::string>();

                FIX42::NewOrderSingle new_order_single;
                new_order_single.set(FIX::ClOrdID(order_id));
                new_order_single.set(FIX::Symbol(symbol));
                FIX::Side fix_side = (side == OrderSide::BUY) ? FIX::Side_BUY : FIX::Side_SELL;
                new_order_single.set(fix_side);
                FIX::TransactTime transact_time;
                transact_time.setString(timestamp);
                new_order_single.set(transact_time);
                new_order_single.set(FIX::OrderQty(std::stod(quantity)));
                FIX::OrdType fix_ord_type = (type == OrderType::LIMIT) ? FIX::OrdType_LIMIT : FIX::OrdType_MARKET;
                new_order_single.set(fix_ord_type);

                if (type == OrderType::LIMIT)
                {
                    new_order_single.set(FIX::Price(std::stod(price)));
                }

                FIX::Session::sendToTarget(new_order_single, session_id);
            }

            work.exec_params(purge_incoming_orders_outbox_query, pqxx::params{ids});
            work.commit();
        }
        catch (const std::exception &e)
        {
        }

        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}