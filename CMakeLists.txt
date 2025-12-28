cmake_minimum_required(VERSION 3.16)

project(network_library
    VERSION 1.0.0
    LANGUAGES C CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(GNUInstallDirs)

# ============================================================
# Global output directories
# ============================================================
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_BINDIR})

# ============================================================
# Dependencies
# ============================================================
find_package(OpenSSL REQUIRED)
find_package(Protobuf REQUIRED)
find_package(PkgConfig REQUIRED)

# --- Detect liburing for io_uring support ---
pkg_check_modules(LIBURING liburing)

if(NOT LIBURING_FOUND)
    message(FATAL_ERROR "liburing not found! Please install liburing-dev.")
endif()

# Ensure imported target exists
if(NOT TARGET PkgConfig::liburing)
    add_library(PkgConfig::liburing INTERFACE IMPORTED)
    target_include_directories(PkgConfig::liburing INTERFACE ${LIBURING_INCLUDE_DIRS})
    target_link_libraries(PkgConfig::liburing INTERFACE ${LIBURING_LIBRARIES})
endif()

# yaml-cpp (config-mode preferred, pkg-config fallback)
find_package(yaml-cpp QUIET)
if(NOT yaml-cpp_FOUND)
    pkg_check_modules(YAML_CPP REQUIRED yaml-cpp)
endif()

# ============================================================
# Make deps visible to subdirectories (READ-ONLY)
# ============================================================
set(YAML_CPP_LIBRARIES
    yaml-cpp
    ${YAML_CPP_LIBRARIES}
    CACHE INTERNAL ""
)

set(YAML_CPP_INCLUDE_DIRS
    ${YAML_CPP_INCLUDE_DIRS}
    CACHE INTERNAL ""
)

# ============================================================
# Subdirectories
# ============================================================
add_subdirectory(api)
add_subdirectory(lib_logger)
add_subdirectory(lib_network)
add_subdirectory(apps)

# ============================================================
# Testing
# ============================================================
option(ENABLE_TESTING "Enable testing" OFF)
if(ENABLE_TESTING)
    enable_testing()
endif()
