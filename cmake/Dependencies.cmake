include(FetchContent)

# Ensure Position Independent Code for all fetched dependencies
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Protobuf
find_package(Protobuf QUIET)
if(NOT Protobuf_FOUND)
    message(STATUS "Protobuf not found, fetching from source...")
    FetchContent_Declare(
        protobuf
        GIT_REPOSITORY https://github.com/protocolbuffers/protobuf.git
        GIT_TAG        v21.12
    )
    set(protobuf_BUILD_TESTS OFF CACHE BOOL "Build tests" FORCE)
    set(protobuf_BUILD_PROTOC_BINARIES ON CACHE BOOL "Build protoc" FORCE)
    FetchContent_MakeAvailable(protobuf)
    set(Protobuf_LIBRARIES protobuf::libprotobuf)
    set(Protobuf_INCLUDE_DIR ${protobuf_SOURCE_DIR}/src)
    set(Protobuf_PROTOC_EXECUTABLE $<TARGET_FILE:protoc>)
else()
    message(STATUS "Found Protobuf: ${Protobuf_VERSION}")
endif()

# OpenSSL
find_package(OpenSSL REQUIRED)

# yaml-cpp
find_package(yaml-cpp QUIET)
if(NOT yaml-cpp_FOUND)
    message(STATUS "yaml-cpp not found, fetching from source...")
    FetchContent_Declare(
        yaml-cpp
        GIT_REPOSITORY https://github.com/jbeder/yaml-cpp.git
        GIT_TAG        0.8.0
    )
    set(YAML_CPP_BUILD_TESTS OFF CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(yaml-cpp)
endif()

# GTest
find_package(GTest QUIET)
if(NOT GTest_FOUND)
    message(STATUS "GTest not found, fetching from source...")
    FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG        v1.14.0
    )
    FetchContent_MakeAvailable(googletest)
endif()
