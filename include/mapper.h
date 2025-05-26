#include "app_execution_report.pb.h"
#include "ExecutionReport.h"

class Mapper
{
public:
    static std::optional<AppExecutionReport> toAppExecutionReport(const FIX42::ExecutionReport &message);
};