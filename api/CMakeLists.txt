cmake_minimum_required(VERSION 3.16)

find_package(Protobuf REQUIRED)

set(PROTO_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/proto)
set(PROTO_GEN_DIR ${PROJECT_SOURCE_DIR}/gen)

file(MAKE_DIRECTORY ${PROTO_GEN_DIR})

file(GLOB PROTO_FILES
    ${PROTO_SRC_DIR}/*.proto
)

set(GENERATED_SRCS)
set(GENERATED_HDRS)

foreach(proto ${PROTO_FILES})
    get_filename_component(proto_name ${proto} NAME_WE)

    list(APPEND GENERATED_SRCS
        ${PROTO_GEN_DIR}/${proto_name}.pb.cc
    )

    list(APPEND GENERATED_HDRS
        ${PROTO_GEN_DIR}/${proto_name}.pb.h
    )
endforeach()

add_custom_command(
    OUTPUT ${GENERATED_SRCS} ${GENERATED_HDRS}
    COMMAND ${Protobuf_PROTOC_EXECUTABLE}
    ARGS
        --cpp_out=${PROTO_GEN_DIR}
        -I ${PROTO_SRC_DIR}
        ${PROTO_FILES}
    DEPENDS ${PROTO_FILES}
    COMMENT "Generating protobuf sources into gen/"
)

add_library(protos STATIC
    ${GENERATED_SRCS}
    ${GENERATED_HDRS}
)

# Make static library position independent
set_target_properties(protos PROPERTIES POSITION_INDEPENDENT_CODE ON)

set_source_files_properties(
    ${GENERATED_SRCS} ${GENERATED_HDRS}
    PROPERTIES GENERATED TRUE
)

target_include_directories(protos
    PUBLIC
        ${PROTO_GEN_DIR}
)

target_link_libraries(protos
    PUBLIC
        protobuf::libprotobuf
)

set_target_properties(protos PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
)
