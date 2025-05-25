#include <concurrentqueue.h>

int main()
{
    moodycamel::ConcurrentQueue<std::string> execution_report_queue;
    moodycamel::ConcurrentQueue<std::string> market_data_update_queue;

    return 0;
}