cmake_minimum_required(VERSION 3.15)
project(exchange_poc LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(FetchContent)

FetchContent_Declare(
    bs_thread_pool
    GIT_REPOSITORY https://github.com/bshoshany/thread-pool.git
    GIT_TAG v4.0.1
)
FetchContent_MakeAvailable(bs_thread_pool)

FetchContent_Declare(
    QuickFIX
    GIT_REPOSITORY https://github.com/quickfix/quickfix.git
    GIT_TAG        master
)
FetchContent_MakeAvailable(QuickFIX)

FetchContent_Declare(
    ConcurrentQueue
    GIT_REPOSITORY https://github.com/cameron314/concurrentqueue.git
    GIT_TAG        master
)
FetchContent_MakeAvailable(ConcurrentQueue)

FetchContent_Declare(
    rabbitmq-c
    GIT_REPOSITORY https://github.com/alanxz/rabbitmq-c.git
    GIT_TAG v0.13.0
)
FetchContent_MakeAvailable(rabbitmq-c)

FetchContent_Declare(
    librdkafka
    GIT_REPOSITORY https://github.com/edenhill/librdkafka.git
    GIT_TAG v2.3.0
)
FetchContent_MakeAvailable(librdkafka)

FetchContent_Declare(
    libpqxx
    GIT_REPOSITORY https://github.com/jtv/libpqxx.git
    GIT_TAG 7.8.1
)
FetchContent_MakeAvailable(libpqxx)



file(GLOB_RECURSE SOURCE_FILES 
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)

add_executable(exchange_poc ${SOURCE_FILES})

target_include_directories(exchange_poc
    PRIVATE
        ${quickfix_SOURCE_DIR}/src/C++
        ${quickfix_SOURCE_DIR}/src/C++/fix42
        ${quickfix_BINARY_DIR}/src/C++
        ${concurrentqueue_SOURCE_DIR}
        ${bs_thread_pool_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(exchange_poc
    PRIVATE
        quickfix
)
