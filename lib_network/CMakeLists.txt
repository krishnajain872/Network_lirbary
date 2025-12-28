cmake_minimum_required(VERSION 3.16)

# Collect sources
file(GLOB_RECURSE NETWORK_SOURCES src/*.cc)

# Define library
add_library(network SHARED ${NETWORK_SOURCES})

# Include directories
target_include_directories(network PUBLIC
    include
    ${CMAKE_SOURCE_DIR}/lib_logger/include
    ${CMAKE_SOURCE_DIR}/api/gen
)

# Dependencies
target_link_libraries(network PUBLIC
    logger
    protos
    OpenSSL::SSL
    OpenSSL::Crypto
    yaml-cpp
    ${Protobuf_LIBRARIES}
)

# Install
install(TARGETS network
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
)
install(DIRECTORY include/network DESTINATION include)
