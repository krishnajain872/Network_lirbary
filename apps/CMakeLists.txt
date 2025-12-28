cmake_minimum_required(VERSION 3.16)

# Client
add_executable(oms_client
    client/main.cc
    client/scenarios/order_placement.cc
    client/scenarios/market_data_consumer.cc
    client/scenarios/benchmark.cc
)
target_link_libraries(oms_client PRIVATE network logger protos)

# Server
add_executable(oms_server
    server/main.cc
)
target_link_libraries(oms_server PRIVATE network logger protos)

# Install
install(TARGETS oms_client oms_server
    RUNTIME DESTINATION bin
)
