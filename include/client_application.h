#include <queue>
#include <quickfix/Application.h>
#include <quickfix/MessageCracker.h>
#include <quickfix/Session.h>
#include <quickfix/SessionID.h>
#include <quickfix/SessionSettings.h>
#include <quickfix/Message.h>
#include <quickfix/DataDictionary.h>
#include "app_execution_report.pb.h"

class ClientApplication : public FIX::Application, public FIX::MessageCracker
{
private:
    std::queue<AppExecutionReport> &execution_report_queue_;
    FIX::SessionID session_id_;

    void process_messages();

public:
    ClientApplication(std::queue<AppExecutionReport> &execution_report_queue);
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