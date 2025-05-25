#include <concurrentqueue.h>

int main()
{
    moodycamel::ConcurrentQueue<std::string> rabbitmq_queue;
    moodycamel::ConcurrentQueue<std::string> kafka_queue;
    moodycamel::ConcurrentQueue<std::string> persistence_queue;

    return 0;
}