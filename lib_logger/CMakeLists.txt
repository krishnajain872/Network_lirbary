cmake_minimum_required(VERSION 3.16)

# Collect sources
file(GLOB_RECURSE LOGGER_SOURCES src/*.cc)

# Define library
add_library(logger SHARED ${LOGGER_SOURCES})

# Include directories
target_include_directories(logger PUBLIC
    include
)

# Dependencies
# Json is header-only, included in include/logger/json.hpp
# If Kafka or Syslog need libraries, link them here.
find_package(Threads REQUIRED)

target_link_libraries(logger PUBLIC
    Threads::Threads
)

# Install
install(TARGETS logger
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
)
install(DIRECTORY include/logger DESTINATION include)

if (BUILD_TESTING)
    enable_testing()
    find_package(GTest REQUIRED)
    add_executable(logger_test tests/logger_test.cc)
    target_link_libraries(logger_test PRIVATE logger GTest::gtest_main)
    target_include_directories(logger_test PRIVATE include)
    add_test(NAME logger_test COMMAND logger_test)
endif()
