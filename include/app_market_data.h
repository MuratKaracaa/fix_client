#pragma once
#include <string>

class AppMarketData
{
private:
    std::string stock_symbol;
    double latest_price;
    std::string timestamp;

public:
    AppMarketData() = default;

    const std::string &get_stock_symbol() const { return stock_symbol; }
    double get_latest_price() const { return latest_price; }
    const std::string &get_timestamp() const { return timestamp; }

    void set_stock_symbol(const std::string &symbol) { stock_symbol = symbol; }
    void set_latest_price(double price) { latest_price = price; }
    void set_timestamp(const std::string &time) { timestamp = time; }
};