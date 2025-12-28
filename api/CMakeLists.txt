cmake_minimum_required(VERSION 3.16)

# Find Protobuf
find_package(Protobuf REQUIRED)

set(PROTO_SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/proto")
set(PROTO_GEN_DIR "${CMAKE_CURRENT_BINARY_DIR}/gen")

file(MAKE_DIRECTORY ${PROTO_GEN_DIR})

file(GLOB PROTO_FILES "${PROTO_SRC_DIR}/*.proto")

# Generate C++ sources
protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS
    EXPORT_MACRO DLLEXPORT
    DESCRIPTORS PROTO_DESCRIPTORS
    PROTOC_OUT_DIR "${PROTO_GEN_DIR}"
    ${PROTO_FILES}
)

# Create a library for the generated code
add_library(protos STATIC ${PROTO_SRCS} ${PROTO_HDRS})

# Ensure Position Independent Code for linking into shared libraries
set_property(TARGET protos PROPERTY POSITION_INDEPENDENT_CODE ON)

# Debugging: Print where we expect headers to be
message(STATUS "PROTO_GEN_DIR: ${PROTO_GEN_DIR}")

target_include_directories(protos PUBLIC
    ${PROTO_GEN_DIR}
    ${PROTO_GEN_DIR}/proto
    ${PROTO_GEN_DIR}/api/proto
    ${Protobuf_INCLUDE_DIR}
)

target_link_libraries(protos PUBLIC ${Protobuf_LIBRARIES})

# Install
install(TARGETS protos
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
)
