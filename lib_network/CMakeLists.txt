# ============================================================
# Network Library (lib_network)
# ============================================================

file(GLOB_RECURSE NETWORK_SOURCES
    ${PROJECT_SOURCE_DIR}/lib_network/src/*.cc
)
# Ensure hpack.cc is picked up
# ${PROJECT_SOURCE_DIR}/lib_network/src/protocols/http2/hpack.cc

add_library(network SHARED
    ${NETWORK_SOURCES}
)

# Include directories
target_include_directories(network
    PUBLIC
        ${PROJECT_SOURCE_DIR}/lib_network/include
        ${PROJECT_SOURCE_DIR}/gen          # protobuf headers
)

# Link dependencies
target_link_libraries(network
    PUBLIC
        protos
        logger
        OpenSSL::SSL
        OpenSSL::Crypto
        yaml-cpp
        PkgConfig::liburing   # io_uring support
)

# Output directories
set_target_properties(network protos PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

# Installation
install(TARGETS network protos
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
)

install(DIRECTORY lib_network/include/
    DESTINATION include
)

install(DIRECTORY gen/
    DESTINATION include
)
