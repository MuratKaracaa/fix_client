#include "mapper.h"
#include <quickfix/FieldMap.h>
#include <quickfix/Field.h>

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