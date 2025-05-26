#include "app_execution_report.pb.h"
#include "app_market_data.h"
#include "ExecutionReport.h"
#include <quickfix/fix42/MarketDataSnapshotFullRefresh.h>

class Mapper
{
public:
    static std::optional<AppExecutionReport> toAppExecutionReport(const FIX42::ExecutionReport &message);
    static std::optional<AppMarketData> toAppMarketData(const FIX42::MarketDataSnapshotFullRefresh &message);
};