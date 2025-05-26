#include "client_application.h"
#include "app_execution_report.pb.h"
#include "mapper.h"

ClientApplication::ClientApplication(moodycamel::ConcurrentQueue<AppExecutionReport> &execution_report_queue, moodycamel::ConcurrentQueue<AppMarketData> &market_data_queue)
    : execution_report_queue_(execution_report_queue),
      market_data_queue_(market_data_queue)
{
}

ClientApplication::~ClientApplication()
{
}

void ClientApplication::onCreate(const FIX::SessionID &session_id)
{
}

void ClientApplication::onLogon(const FIX::SessionID &session_id)
{

    session_id_ = session_id;
}

void ClientApplication::onLogout(const FIX::SessionID &session_id)
{
}

void ClientApplication::toAdmin(FIX::Message &message, const FIX::SessionID &session_id)
{
}

void ClientApplication::toApp(FIX::Message &message, const FIX::SessionID &session_id)
{
}

void ClientApplication::fromAdmin(const FIX::Message &message, const FIX::SessionID &session_id)
{
}

void ClientApplication::fromApp(const FIX::Message &message, const FIX::SessionID &session_id)
{
    crack(message, session_id);
}

void ClientApplication::onMessage(const FIX42::ExecutionReport &message, const FIX::SessionID &session_id)
{
    std::optional<AppExecutionReport> optional_app_execution_report = Mapper::toAppExecutionReport(message);
    if (optional_app_execution_report)
    {
        AppExecutionReport app_execution_report = std::move(*optional_app_execution_report);
        execution_report_queue_.enqueue(app_execution_report);
    }
}

void ClientApplication::onMessage(const FIX42::MarketDataSnapshotFullRefresh &message, const FIX::SessionID &session_id)
{
    std::optional<AppMarketData> optional_app_market_data = Mapper::toAppMarketData(message);
    if (optional_app_market_data)
    {
        AppMarketData app_market_data = std::move(*optional_app_market_data);
        market_data_queue_.enqueue(app_market_data);
    }
}