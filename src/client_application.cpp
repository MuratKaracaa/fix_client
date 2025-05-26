#include "client_application.h"
#include <app_execution_report.pb.h>
#include "mapper.h"

ClientApplication::ClientApplication(std::queue<AppExecutionReport> &execution_report_queue)
    : execution_report_queue_(execution_report_queue)
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
        execution_report_queue_.push(app_execution_report);
    }
}

void ClientApplication::onMessage(const FIX42::MarketDataSnapshotFullRefresh &message, const FIX::SessionID &session_id)
{
}