cmake_minimum_required(VERSION 3.15)
project(fix_client LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(FetchContent)

FetchContent_Declare(
    abseil
    GIT_REPOSITORY https://github.com/abseil/abseil-cpp.git
    GIT_TAG 20240116.2
)

set(ABSL_ENABLE_INSTALL ON)
set(ABSL_PROPAGATE_CXX_STD ON)
FetchContent_MakeAvailable(abseil)

FetchContent_Declare(
    protobuf
    GIT_REPOSITORY https://github.com/protocolbuffers/protobuf.git
    GIT_TAG v29.3
)

set(protobuf_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(protobuf_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(protobuf_BUILD_PROTOC_BINARIES ON CACHE BOOL "" FORCE)
set(protobuf_ABSL_PROVIDER "package" CACHE STRING "" FORCE)
FetchContent_MakeAvailable(protobuf)

FetchContent_Declare(
    bs_thread_pool
    GIT_REPOSITORY https://github.com/bshoshany/thread-pool.git
    GIT_TAG v5.0.0
)
FetchContent_MakeAvailable(bs_thread_pool)

FetchContent_Declare(
    QuickFIX
    GIT_REPOSITORY https://github.com/quickfix/quickfix.git
    GIT_TAG master
)
FetchContent_MakeAvailable(QuickFIX)

FetchContent_Declare(
    ConcurrentQueue
    GIT_REPOSITORY https://github.com/cameron314/concurrentqueue.git
    GIT_TAG v1.0.4
)
FetchContent_MakeAvailable(ConcurrentQueue)

FetchContent_Declare(
    librdkafka
    GIT_REPOSITORY https://github.com/edenhill/librdkafka.git
    GIT_TAG v2.10.0
)
FetchContent_MakeAvailable(librdkafka)

FetchContent_Declare(
    hiredis
    GIT_REPOSITORY https://github.com/redis/hiredis.git
    GIT_TAG v1.3.0
)
FetchContent_MakeAvailable(hiredis)

FetchContent_Declare(
    libpqxx
    GIT_REPOSITORY https://github.com/jtv/libpqxx.git
    GIT_TAG 7.10.1
)
FetchContent_MakeAvailable(libpqxx)

file(GLOB_RECURSE SOURCE_FILES 
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)

file(GLOB_RECURSE GENERATED_FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/gen/*.pb.cc"
    "${CMAKE_CURRENT_SOURCE_DIR}/gen/*.pb.h"
)

add_executable(fix_client ${SOURCE_FILES} ${GENERATED_FILES})

target_include_directories(fix_client
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${CMAKE_CURRENT_SOURCE_DIR}/gen
        ${bs_thread_pool_SOURCE_DIR}/include
        ${quickfix_SOURCE_DIR}/src/C++
        ${hiredis_SOURCE_DIR}
        ${quickfix_SOURCE_DIR}/src/C++/fix42
        ${quickfix_BINARY_DIR}/src/C++
        ${concurrentqueue_SOURCE_DIR}
        ${librdkafka_SOURCE_DIR}/src
        ${libpqxx_SOURCE_DIR}/include
        ${libpqxx_BINARY_DIR}/include
)

target_link_libraries(fix_client
    PRIVATE
        quickfix
        hiredis
        rdkafka
        pqxx
        pthread
        protobuf::libprotobuf
        absl::log
        absl::log_internal_check_op
        absl::log_internal_message
)