cmake_minimum_required(VERSION 3.16)
project(network-library VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

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
set(CORE_SOURCES
    core/src/event/event_loop.cpp
    core/src/event/socket.cpp
    core/src/event/reactor.cpp
    core/src/memory/buffer.cpp
    core/src/memory/buffer_pool.cpp
    core/src/connection.cpp
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
    core/src/protocols/rpc/rpc_handler.cpp
    core/src/config/config_parser.cpp
    core/src/config/config_hot_reload.cpp
    core/src/resilience/token_bucket.cpp
    core/src/resilience/circuit_breaker.cpp
    core/src/security/ddos_protection.cpp
    core/src/event/io_uring_poller.cpp
    core/src/observability/metrics/metrics_collector.cpp
    core/src/observability/tracing/tracer.cpp
    core/src/observability/tracing/otlp_exporter.cpp
    core/src/event/udp_socket.cpp
    core/src/protocols/udp/udp_handler.cpp
    ${PROTO_SRCS}
)

# --------------------------------------------------
# Main Server Executable
# --------------------------------------------------
add_executable(network-server
    core/src/server_main.cpp
    ${CORE_SOURCES}
)

target_link_libraries(network-server
    ${Protobuf_LIBRARIES}
    OpenSSL::SSL
    OpenSSL::Crypto
    yaml-cpp
)

# ✅ INSTALL THE SERVER (THIS FIXES YOUR DOCKER ERROR)
install(TARGETS network-server
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# HTTPS Integration Test
add_executable(test_https_integration
    tests/integration/test_https_integration.cpp
    ${CORE_SOURCES}
)
target_link_libraries(test_https_integration
    ${Protobuf_LIBRARIES}
    OpenSSL::SSL
    OpenSSL::Crypto
    yaml-cpp
)

install(TARGETS test_https_integration
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# HTTP/2 Frame Test
add_executable(test_http2_frame
    tests/unit/protocols/test_http2_frame.cpp
    core/src/protocols/http2/frame_parser.cpp
    core/src/memory/buffer.cpp
    core/src/memory/buffer_pool.cpp
)

install(TARGETS test_http2_frame
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# gRPC Codec Test
add_executable(test_grpc_codec
    tests/unit/protocols/test_grpc_codec.cpp
    core/src/protocols/grpc/grpc_codec.cpp
    core/src/memory/buffer.cpp
    core/src/memory/buffer_pool.cpp
)

install(TARGETS test_grpc_codec
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# gRPC E2E Test
add_executable(test_grpc_e2e
    tests/integration/test_grpc_e2e.cpp
    ${CORE_SOURCES}
)
target_link_libraries(test_grpc_e2e
    ${Protobuf_LIBRARIES}
    OpenSSL::SSL
    OpenSSL::Crypto
    yaml-cpp
)

install(TARGETS test_grpc_e2e
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# WebSocket Test
add_executable(test_websocket
    tests/unit/protocols/test_websocket.cpp
    core/src/protocols/websocket/frame_parser.cpp
)

install(TARGETS test_websocket
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# Stream Envelope Test
add_executable(test_stream_envelope
    tests/unit/protocols/test_stream_envelope.cpp
    ${PROTO_SRCS}
)
target_link_libraries(test_stream_envelope
    ${Protobuf_LIBRARIES}
)

install(TARGETS test_stream_envelope
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# UDP Echo Test
add_executable(test_udp_echo
    tests/integration/test_udp_echo.cpp
    ${CORE_SOURCES}
)
target_link_libraries(test_udp_echo
    ${Protobuf_LIBRARIES}
    OpenSSL::SSL
    OpenSSL::Crypto
    yaml-cpp
)

install(TARGETS test_udp_echo
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# OMS Reference Application
add_executable(oms_app
    examples/oms/oms_app.cpp
    ${CORE_SOURCES}
)
target_include_directories(oms_app PRIVATE 
    examples/oms
    ${CMAKE_CURRENT_BINARY_DIR} # Include generated protos
)
target_link_libraries(oms_app
    ${Protobuf_LIBRARIES}
    OpenSSL::SSL
    OpenSSL::Crypto
    yaml-cpp
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
    ${CORE_SOURCES}
)
target_include_directories(oms_terminal PRIVATE 
    examples/oms
    ${CMAKE_CURRENT_BINARY_DIR} # Include generated protos
)
target_link_libraries(oms_terminal
    ${Protobuf_LIBRARIES}
    OpenSSL::SSL
    OpenSSL::Crypto
    yaml-cpp
)
install(TARGETS oms_terminal
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# Resilience Test
add_executable(test_resilience
    tests/unit/core/test_resilience.cpp
    core/src/resilience/token_bucket.cpp
    core/src/resilience/circuit_breaker.cpp
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
    core/src/config/config_parser.cpp
)
target_link_libraries(test_config yaml-cpp)

install(TARGETS test_config
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# Full Config Test
add_executable(test_full_config
    tests/unit/config/test_full_config.cpp
    core/src/config/config_parser.cpp
)
target_link_libraries(test_full_config yaml-cpp GTest::gtest_main)

install(TARGETS test_full_config
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# Event Loop Test
add_executable(test_event_loop
    tests/unit/event/test_event_loop.cpp
    core/src/event/event_loop.cpp
    core/src/memory/buffer.cpp
    core/src/memory/buffer_pool.cpp
)
target_include_directories(test_event_loop PUBLIC core/include)

install(TARGETS test_event_loop
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# Buffer Test
add_executable(test_buffer
    tests/unit/memory/test_buffer.cpp
    core/src/memory/buffer.cpp
    core/src/memory/buffer_pool.cpp
)

install(TARGETS test_buffer
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# HTTP Integration Test
add_executable(test_http_integration
    tests/integration/test_http_integration.cpp
    ${CORE_SOURCES}
)
target_link_libraries(test_http_integration
    ${Protobuf_LIBRARIES}
    OpenSSL::SSL
    OpenSSL::Crypto
    yaml-cpp
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
    core/src/memory/buffer_pool.cpp
)

install(TARGETS memory_benchmark
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# --------------------------------------------------
# Python Bindings
# --------------------------------------------------
pybind11_add_module(networklib_py
    bindings/python/networklib_py.cpp
    ${CORE_SOURCES}
)
target_link_libraries(networklib_py PRIVATE
    ${Protobuf_LIBRARIES}
    OpenSSL::SSL
    OpenSSL::Crypto
    yaml-cpp
)
install(TARGETS networklib_py
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)
