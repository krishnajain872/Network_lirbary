enable_testing()
include(GoogleTest)

# --------------------------------------------------
# Test name / source pairs
# --------------------------------------------------
set(TESTS
    test_https_integration   tests/integration/test_https_integration.cpp
    test_http_integration    tests/integration/test_http_integration.cpp
    test_grpc_e2e            tests/integration/test_grpc_e2e.cpp
    test_udp_echo            tests/integration/test_udp_echo.cpp

    test_http2_frame         tests/unit/protocols/test_http2_frame.cpp
    test_grpc_codec          tests/unit/protocols/test_grpc_codec.cpp
    test_websocket           tests/unit/protocols/test_websocket.cpp
    test_stream_envelope     tests/unit/protocols/test_stream_envelope.cpp

    test_resilience          tests/unit/core/test_resilience.cpp
    test_event_loop          tests/unit/event/test_event_loop.cpp
    test_buffer              tests/unit/memory/test_buffer.cpp
)

# --------------------------------------------------
# SAFE iteration (NO out-of-range possible)
# --------------------------------------------------
list(LENGTH TESTS _len)
set(_idx 0)

while(_idx LESS _len)
    list(GET TESTS ${_idx} test_name)
    math(EXPR _idx "${_idx} + 1")

    list(GET TESTS ${_idx} test_src)
    math(EXPR _idx "${_idx} + 1")

    if(NOT TARGET ${test_name})
        add_network_exec(${test_name} ${test_src})
    endif()
endwhile()

# --------------------------------------------------
# GTest-based tests
# --------------------------------------------------
if(NOT TARGET test_logging)
    add_network_exec(test_logging tests/unit/logging/test_logging.cpp)
    target_link_libraries(test_logging PRIVATE GTest::gtest_main)
endif()

if(NOT TARGET test_full_config)
    add_network_exec(test_full_config tests/unit/config/test_full_config.cpp)
    target_link_libraries(test_full_config PRIVATE GTest::gtest_main)
    gtest_discover_tests(test_full_config)
endif()

# --------------------------------------------------
# Performance Tests
# --------------------------------------------------
if(NOT TARGET connection_stress_test)
    add_network_exec(connection_stress_test tests/performance/connection_tests.cpp)
endif()

if(NOT TARGET memory_benchmark)
    add_network_exec(memory_benchmark tests/performance/memory_benchmark.cpp)
endif()
