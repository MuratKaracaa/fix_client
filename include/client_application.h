#pragma once
#include "Application.h"
#include "MessageCracker.h"
#include "Session.h"
#include "SessionID.h"
#include "SessionSettings.h"
#include "Message.h"
#include "DataDictionary.h"
#include "app_execution_report.pb.h"
#include "concurrentqueue.h"
#include "app_market_data.h"

class ClientApplication : public FIX::Application, public FIX::MessageCracker
{
private:
    moodycamel::ConcurrentQueue<AppExecutionReport> &execution_report_queue_;
    moodycamel::ConcurrentQueue<AppMarketData> &market_data_queue_;
    FIX::SessionID session_id_;

    void process_messages();

public:
    ClientApplication(moodycamel::ConcurrentQueue<AppExecutionReport> &execution_report_queue, moodycamel::ConcurrentQueue<AppMarketData> &market_data_queue);
    ~ClientApplication();

    void onCreate(const FIX::SessionID &session_id) override;
    void onLogon(const FIX::SessionID &session_id) override;
    void onLogout(const FIX::SessionID &session_id) override;
    void toAdmin(FIX::Message &message, const FIX::SessionID &session_id) override;
    void toApp(FIX::Message &message, const FIX::SessionID &session_id) override;
    void fromAdmin(const FIX::Message &message, const FIX::SessionID &session_id) override;
    void fromApp(const FIX::Message &message, const FIX::SessionID &session_id) override;
    void onMessage(const FIX42::ExecutionReport &message, const FIX::SessionID &session_id) override;
    void onMessage(const FIX42::MarketDataSnapshotFullRefresh &message, const FIX::SessionID &session_id) override;
};