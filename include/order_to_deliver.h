#include <string>
#include "order_side.h"
#include "order_type.h"
class OrderToDeliver
{
public:
    OrderToDeliver(const std::string &order_id, const std::string &symbol, OrderSide side, OrderType type, const std::string &quantity, const std::string &price, const std::string &timestamp);
    ~OrderToDeliver();

private:
    std::string order_id;
    std::string symbol;
    OrderSide side;
    OrderType type;
    std::string quantity;
    std::string price;
    std::string timestamp;
};