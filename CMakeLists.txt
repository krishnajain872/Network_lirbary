cmake_minimum_required(VERSION 3.16)
project(network-library VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# --------------------------------------------------
# Optimizations & Build Modes
# --------------------------------------------------
# 1. CCache Support
find_program(CCACHE_PROGRAM ccache)
if(CCACHE_PROGRAM)
    message(STATUS "Using ccache: ${CCACHE_PROGRAM}")
    set(CMAKE_CXX_COMPILER_LAUNCHER "${CCACHE_PROGRAM}")
    set(CMAKE_C_COMPILER_LAUNCHER "${CCACHE_PROGRAM}")
endif()

# 2. Build Types (Debug, Release, ASan)
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    message(STATUS "Setting build type to 'Release' as none was specified.")
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build." FORCE)
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "MinSizeRel" "RelWithDebInfo" "ASan")
endif()

set(CMAKE_CXX_FLAGS_DEBUG "-O0 -g -Wall -Wextra -DDEBUG")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG -march=native -flto")
set(CMAKE_CXX_FLAGS_ASAN "-O1 -g -fsanitize=address -fno-omit-frame-pointer -DDEBUG")
set(CMAKE_EXE_LINKER_FLAGS_ASAN "-fsanitize=address")
set(CMAKE_SHARED_LINKER_FLAGS_ASAN "-fsanitize=address")
set(CMAKE_MODULE_LINKER_FLAGS_ASAN "-fsanitize=address")

# 3. Interprocedural Optimization (LTO)
include(CheckIPOSupported)
check_ipo_supported(RESULT result OUTPUT output)
if(result AND "${CMAKE_BUILD_TYPE}" STREQUAL "Release")
    set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)
endif()

# 4. Unity Build (Jumbo Build) - Optional
option(ENABLE_UNITY_BUILD "Enable Unity Build (faster compilation)" OFF)
if(ENABLE_UNITY_BUILD)
    set(CMAKE_UNITY_BUILD ON)
endif()

# --------------------------------------------------
# Install directories (BEST PRACTICE)
# --------------------------------------------------
include(GNUInstallDirs)

# --------------------------------------------------
# Dependencies
# --------------------------------------------------
find_package(Protobuf REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(yaml-cpp REQUIRED)
find_package(GTest REQUIRED)
find_package(pybind11 REQUIRED)

# --------------------------------------------------
# Include directories
# --------------------------------------------------
include_directories(
    core/include
    ${CMAKE_CURRENT_BINARY_DIR} # Generated protobuf headers
    ${Protobuf_INCLUDE_DIRS}
    ${OPENSSL_INCLUDE_DIR}
    ${YAML_CPP_INCLUDE_DIR}
)

# --------------------------------------------------
# Protobuf generation
# --------------------------------------------------
# Ensure oms_definitions.proto is compiled
file(GLOB PROTO_FILES "protocols/proto/*.proto")
protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS ${PROTO_FILES})

# --------------------------------------------------
# Core Sources
# --------------------------------------------------
set(LOGGING_SOURCES
    core/src/logging/log_config.cpp
    core/src/logging/async_logger.cpp
    core/src/logging/logger.cpp
)

set(CORE_SOURCES
    core/src/event/event_loop.cpp
    core/src/event/socket.cpp
    core/src/event/reactor.cpp
    core/src/memory/buffer.cpp
    core/src/memory/buffer_pool.cpp
    core/src/connection.cpp
    core/src/server.cpp
    core/src/client.cpp
    core/src/client/protocols/client_protocol.cpp
    core/src/network_lib.cpp
    core/src/security/tls_context.cpp
    core/src/protocols/protocol_factory.cpp
    core/src/protocols/tcp/tcp_handler.cpp
    core/src/protocols/http/http_handler.cpp
    core/src/protocols/http2/frame_parser.cpp
    core/src/protocols/http2/stream.cpp
    core/src/protocols/http2/session.cpp
    core/src/protocols/grpc/grpc_handler.cpp
    core/src/protocols/grpc/grpc_codec.cpp
    core/src/protocols/grpc/service_registry.cpp
    core/src/protocols/websocket/handshake.cpp
    core/src/protocols/websocket/frame_parser.cpp
    core/src/protocols/websocket/websocket_handler.cpp
    core/src/protocols/quic/quic_handler.cpp
    core/src/protocols/rpc/rpc_handler.cpp
    core/src/config/config_parser.cpp
    core/src/config/config_hot_reload.cpp
    core/src/resilience/token_bucket.cpp
    core/src/resilience/circuit_breaker.cpp
    core/src/security/ddos_protection.cpp
    core/src/event/epoll_poller.cpp
    core/src/event/io_uring_poller.cpp
    core/src/observability/metrics/metrics_collector.cpp
    core/src/observability/tracing/tracer.cpp
    core/src/observability/tracing/otlp_exporter.cpp
    core/src/event/udp_socket.cpp
    core/src/protocols/udp/udp_handler.cpp
    ${LOGGING_SOURCES}
    ${PROTO_SRCS}
)

# --------------------------------------------------
# Main Library (Dynamic by default)
# --------------------------------------------------
option(BUILD_SHARED_LIBS "Build shared libraries" ON)

add_library(networklib ${CORE_SOURCES})

# Optimization: Precompiled Headers
target_precompile_headers(networklib PRIVATE
    <vector>
    <string>
    <memory>
    <map>
    <thread>
    <mutex>
    <atomic>
    <iostream>
    <functional>
    <cstring>
    <algorithm>
)

target_include_directories(networklib PUBLIC
    core/include
    core/include/networklib/external
    ${CMAKE_CURRENT_BINARY_DIR} # Generated protobuf headers
    ${Protobuf_INCLUDE_DIRS}
    ${OPENSSL_INCLUDE_DIR}
    ${YAML_CPP_INCLUDE_DIR}
)

target_link_libraries(networklib PUBLIC
    ${Protobuf_LIBRARIES}
    OpenSSL::SSL
    OpenSSL::Crypto
    yaml-cpp
    uring
)


install(TARGETS networklib
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

# --------------------------------------------------
# Main Server Executable
# --------------------------------------------------
add_executable(network-server
    core/src/server_main.cpp
)

target_link_libraries(network-server
    networklib
)

# ✅ INSTALL THE SERVER (THIS FIXES YOUR DOCKER ERROR)
install(TARGETS network-server
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# HTTPS Integration Test
add_executable(test_https_integration
    tests/integration/test_https_integration.cpp
)
target_link_libraries(test_https_integration
    networklib
)

install(TARGETS test_https_integration
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# HTTP/2 Frame Test
add_executable(test_http2_frame
    tests/unit/protocols/test_http2_frame.cpp
)
target_link_libraries(test_http2_frame
    networklib
)

install(TARGETS test_http2_frame
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# gRPC Codec Test
add_executable(test_grpc_codec
    tests/unit/protocols/test_grpc_codec.cpp
)
target_link_libraries(test_grpc_codec
    networklib
)

install(TARGETS test_grpc_codec
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# gRPC E2E Test
add_executable(test_grpc_e2e
    tests/integration/test_grpc_e2e.cpp
)
target_link_libraries(test_grpc_e2e
    networklib
)

install(TARGETS test_grpc_e2e
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# WebSocket Test
add_executable(test_websocket
    tests/unit/protocols/test_websocket.cpp
)
target_link_libraries(test_websocket
    networklib
)

install(TARGETS test_websocket
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# Stream Envelope Test
add_executable(test_stream_envelope
    tests/unit/protocols/test_stream_envelope.cpp
)
target_link_libraries(test_stream_envelope
    networklib
)

install(TARGETS test_stream_envelope
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# UDP Echo Test
add_executable(test_udp_echo
    tests/integration/test_udp_echo.cpp
)
target_link_libraries(test_udp_echo
    networklib
)

install(TARGETS test_udp_echo
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# Generic Server Example
add_executable(generic_server
    examples/generic_server/main.cpp
)
target_link_libraries(generic_server
    networklib
)
install(TARGETS generic_server
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# Generic Client Example
add_executable(generic_client
    examples/generic_client/main.cpp
)
target_link_libraries(generic_client
    networklib
)
install(TARGETS generic_client
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# OMS Reference Application
add_executable(oms_app
    examples/oms/oms_app.cpp
)
target_include_directories(oms_app PRIVATE 
    examples/oms
    ${CMAKE_CURRENT_BINARY_DIR} # Include generated protos
)
target_link_libraries(oms_app
    networklib
)

install(TARGETS oms_app
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# OMS Load Test
add_executable(oms_load_test
    examples/oms/load_test.cpp
)
install(TARGETS oms_load_test
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# OMS Terminal Client
add_executable(oms_terminal
    examples/oms/terminal/terminal_app.cpp
)
target_include_directories(oms_terminal PRIVATE 
    examples/oms
    ${CMAKE_CURRENT_BINARY_DIR} # Include generated protos
)
target_link_libraries(oms_terminal
    networklib
)
install(TARGETS oms_terminal
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# Resilience Test
add_executable(test_resilience
    tests/unit/core/test_resilience.cpp
)
target_link_libraries(test_resilience
    networklib
)

install(TARGETS test_resilience
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# --------------------------------------------------
# Unit Tests
# --------------------------------------------------
enable_testing()

# Config Test
add_executable(test_config
    tests/unit/config/test_config.cpp
)
target_link_libraries(test_config networklib yaml-cpp)

install(TARGETS test_config
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# Full Config Test
add_executable(test_full_config
    tests/unit/config/test_full_config.cpp
)
target_link_libraries(test_full_config networklib yaml-cpp GTest::gtest_main)
include(GoogleTest)
gtest_discover_tests(test_full_config)

install(TARGETS test_full_config
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# Event Loop Test
add_executable(test_event_loop
    tests/unit/event/test_event_loop.cpp
)
target_link_libraries(test_event_loop networklib)
target_include_directories(test_event_loop PUBLIC core/include)

install(TARGETS test_event_loop
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# Buffer Test
add_executable(test_buffer
    tests/unit/memory/test_buffer.cpp
)
target_link_libraries(test_buffer networklib)

install(TARGETS test_buffer
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# HTTP Integration Test
add_executable(test_http_integration
    tests/integration/test_http_integration.cpp
)
target_link_libraries(test_http_integration
    networklib
)

install(TARGETS test_http_integration
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# --------------------------------------------------
# Performance Test Tool
# --------------------------------------------------
add_executable(connection_stress_test
    tests/performance/connection_tests.cpp
)

install(TARGETS connection_stress_test
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# Memory Benchmark
add_executable(memory_benchmark
    tests/performance/memory_benchmark.cpp
)
target_link_libraries(memory_benchmark networklib)

install(TARGETS memory_benchmark
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# Logging Test
add_executable(test_logging
    tests/unit/logging/test_logging.cpp
)
target_link_libraries(test_logging
    networklib
    GTest::gtest_main
)
install(TARGETS test_logging
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# Logging Example
add_executable(logging_example
    examples/logging/logging_example.cpp
)
target_link_libraries(logging_example
    networklib
)
install(TARGETS logging_example
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# --------------------------------------------------
# Python Bindings
# --------------------------------------------------
pybind11_add_module(networklib_py
    bindings/python/networklib_py.cpp
)
target_link_libraries(networklib_py PRIVATE
    networklib
)
install(TARGETS networklib_py
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)
