#include "mapper.h"
#include "FieldMap.h"
#include "Field.h"
#include "ExecutionReport.h"
#include "MarketDataSnapshotFullRefresh.h"

std::optional<AppExecutionReport> Mapper::toAppExecutionReport(const FIX42::ExecutionReport &message)
{
    try
    {
        AppExecutionReport app_execution_report;

        FIX::OrderID orderID;
        message.get(orderID);
        app_execution_report.set_order_id(orderID.getValue());

        FIX::CumQty volume;
        message.get(volume);
        app_execution_report.set_volume(volume.getValue());

        FIX::TransactTime timestamp;
        message.get(timestamp);
        app_execution_report.set_timestamp(timestamp.getString());

        return app_execution_report;
    }
    catch (const std::exception &e)
    {
        return std::nullopt;
    }
}

std::optional<AppMarketData> Mapper::toAppMarketData(const FIX42::MarketDataSnapshotFullRefresh &message)
{
    try
    {
        AppMarketData app_market_data;

        FIX::Symbol symbol;
        message.get(symbol);
        app_market_data.set_stock_symbol(symbol.getValue());

        FIX::NoMDEntries noMDEntries;
        message.get(noMDEntries);

        for (int i = 1; i <= noMDEntries; ++i)
        {
            FIX42::MarketDataSnapshotFullRefresh::NoMDEntries group;
            message.getGroup(i, group);

            FIX::MDEntryType entryType;
            group.get(entryType);

            if (entryType.getValue() == '2')
            {
                FIX::MDEntryPx price;
                group.get(price);
                app_market_data.set_latest_price(price.getValue());

                std::string timestamp;

                try
                {
                    FIX::MDEntryDate entryDate;
                    group.get(entryDate);

                    FIX::MDEntryTime entryTime;
                    group.get(entryTime);

                    timestamp = entryDate.getString() + "-" + entryTime.getString();
                }
                catch (const std::exception &e)
                {
                }

                app_market_data.set_timestamp(timestamp);

                break;
            }
        }

        return app_market_data;
    }
    catch (const std::exception &e)
    {
        return std::nullopt;
    }
}