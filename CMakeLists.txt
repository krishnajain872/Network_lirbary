cmake_minimum_required(VERSION 3.16)
project(network-library VERSION 1.0.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(GNUInstallDirs)

# Output directories
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Dependencies
find_package(OpenSSL REQUIRED)
find_package(Protobuf REQUIRED)
find_package(PkgConfig REQUIRED)

# Check for yaml-cpp
find_package(yaml-cpp QUIET)
if(NOT yaml-cpp_FOUND)
    pkg_check_modules(YAML_CPP yaml-cpp REQUIRED)
    link_directories(${YAML_CPP_LIBRARY_DIRS})
    include_directories(${YAML_CPP_INCLUDE_DIRS})
endif()

# Subdirectories
add_subdirectory(api)
add_subdirectory(lib_logger)
add_subdirectory(lib_network)
add_subdirectory(apps)

# Testing (optional, if needed)
option(ENABLE_TESTING "Enable testing" OFF)
if(ENABLE_TESTING)
    enable_testing()
    # Add test subdirectories if they exist and have CMakeLists
endif()
