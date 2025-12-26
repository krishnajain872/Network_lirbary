cmake_minimum_required(VERSION 3.16)
project(network-library VERSION 1.0.0 LANGUAGES C CXX)

include(GNUInstallDirs)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

include(BuildOptions)
include(Dependencies)
include(Helpers)

file(GLOB PROTO_FILES protocols/proto/*.proto)
protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS ${PROTO_FILES})

file(GLOB_RECURSE CORE_SOURCES core/src/*.cpp)
list(FILTER CORE_SOURCES EXCLUDE REGEX "core/src/server_main.cpp")

option(BUILD_SHARED_LIBS "Build shared libraries" ON)

add_library(networklib ${CORE_SOURCES} ${PROTO_SRCS})

target_precompile_headers(networklib PRIVATE
    <vector> <string> <memory> <map>
    <thread> <mutex> <atomic>
    <functional> <algorithm>
)

target_include_directories(networklib PUBLIC
    core/include
    ${CMAKE_CURRENT_BINARY_DIR}
)

target_link_libraries(networklib PUBLIC
    network_options
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

add_network_exec(network-server core/src/server_main.cpp)

add_network_exec(generic_server examples/generic_server/main.cpp)
add_network_exec(generic_client examples/generic_client/main.cpp)
add_network_exec(logging_example examples/logging/logging_example.cpp)

add_network_exec(oms_app examples/oms/oms_app.cpp)
add_network_exec(oms_load_test examples/oms/load_test.cpp)
add_network_exec(oms_terminal examples/oms/terminal/terminal_app.cpp)

include(Tests)

pybind11_add_module(networklib_py bindings/python/networklib_py.cpp)
target_link_libraries(networklib_py PRIVATE networklib network_options)
install(TARGETS networklib_py LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
