#pragma once
#include <string>
#include "NewOrderSingle.h"
class OrderToDeliver
{
public:
    OrderToDeliver(const std::string &order_id, const std::string &symbol, FIX::Side side, FIX::OrdType type, const std::string &quantity, const std::string &price, const std::string &timestamp);
    ~OrderToDeliver();

private:
    std::string order_id;
    std::string symbol;
    FIX::Side side;
    FIX::OrdType type;
    std::string quantity;
    std::string price;
    std::string timestamp;
};