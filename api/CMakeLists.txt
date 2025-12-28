cmake_minimum_required(VERSION 3.16)

find_package(Protobuf REQUIRED)

set(PROTO_SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/proto")
set(PROTO_GEN_DIR "${CMAKE_CURRENT_BINARY_DIR}/gen")

file(MAKE_DIRECTORY ${PROTO_GEN_DIR})

file(GLOB PROTO_FILES "${PROTO_SRC_DIR}/*.proto")

if(PROTO_FILES STREQUAL "")
    message(FATAL_ERROR "No .proto files found in ${PROTO_SRC_DIR}")
endif()

protobuf_generate_cpp(
    PROTO_SRCS
    PROTO_HDRS
    ${PROTO_FILES}
)

add_library(protos SHARED
    ${PROTO_SRCS}
    ${PROTO_HDRS}
)

target_include_directories(protos PUBLIC
    ${PROTO_GEN_DIR}
)

target_link_libraries(protos PUBLIC
    protobuf::libprotobuf
)
