cmake_minimum_required(VERSION 3.16)
project(network-library VERSION 1.0.0 LANGUAGES C CXX)

include(GNUInstallDirs)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

include(BuildOptions)
include(Dependencies)
include(Helpers)

# ==============================================================================
# PROTOCOL BUFFERS
# ==============================================================================
file(GLOB PROTO_FILES protocols/proto/*.proto)
if(TARGET protobuf::protoc)
    set(protobuf_PROTOC_EXE protobuf::protoc)
else()
    set(protobuf_PROTOC_EXE ${Protobuf_PROTOC_EXECUTABLE})
endif()

protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS ${PROTO_FILES})

# ==============================================================================
# CORE SOURCES
# ==============================================================================
file(GLOB_RECURSE CORE_SOURCES core/src/*.cpp)
list(FILTER CORE_SOURCES EXCLUDE REGEX "core/src/server_main.cpp")
list(FILTER CORE_SOURCES EXCLUDE REGEX "core/src/event/io_uring_poller.cpp")

# ==============================================================================
# MAIN LIBRARY (libnetwork.so)
# ==============================================================================
add_library(network SHARED ${CORE_SOURCES} ${PROTO_SRCS})

target_precompile_headers(network PRIVATE
    <vector> <string> <memory> <map>
    <thread> <mutex> <atomic>
    <functional> <algorithm>
)

target_include_directories(network PUBLIC
    core/include
    core/include/networklib # To allow #include "detail/interfaces.h" via "networklib/detail/interfaces.h"? No, root is core/include.
    ${CMAKE_CURRENT_BINARY_DIR}
    ${Protobuf_INCLUDE_DIR}
)

target_link_libraries(network PUBLIC
    network_options
    ${Protobuf_LIBRARIES}
    OpenSSL::SSL
    OpenSSL::Crypto
    yaml-cpp
)

install(TARGETS network
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

# ==============================================================================
# APPLICATIONS
# ==============================================================================

# OMS Server
add_executable(oms_server oms_server/main.cpp)
target_link_libraries(oms_server PRIVATE network)
target_include_directories(oms_server PRIVATE core/include/networklib oms_server/proto)

# OMS Client
add_executable(oms_client
    oms_client/main.cpp
    oms_client/scenarios/order_placement.cpp
    oms_client/scenarios/market_data_consumer.cpp
)
target_link_libraries(oms_client PRIVATE network)
target_include_directories(oms_client PRIVATE core/include/networklib)

# ==============================================================================
# TESTS
# ==============================================================================
enable_testing()

# Unit Tests
file(GLOB UNIT_TEST_SRCS tests/unit/*.cpp)
if(UNIT_TEST_SRCS)
    add_executable(unit_tests ${UNIT_TEST_SRCS})
    target_link_libraries(unit_tests PRIVATE network GTest::gtest_main)
    target_include_directories(unit_tests PRIVATE core/include/networklib)
    add_test(NAME UnitTests COMMAND unit_tests)
endif()

# Integration Tests
file(GLOB INTEGRATION_TEST_SRCS tests/integration/*.cpp)
if(INTEGRATION_TEST_SRCS)
    add_executable(integration_tests ${INTEGRATION_TEST_SRCS})
    target_link_libraries(integration_tests PRIVATE network GTest::gtest_main)
    target_include_directories(integration_tests PRIVATE core/include/networklib)
    add_test(NAME IntegrationTests COMMAND integration_tests)
endif()
