cmake_minimum_required(VERSION 3.10)
project(forex_cycles)

set(CMAKE_CXX_STANDARD 17)

# Find igraph if it was installed globally (in /usr/local)
find_package(igraph REQUIRED)

include_directories(
    src
    src/engine
    src/rate_service
    src/simulation
    src/currency_service
)

add_executable(forex_cycles
    src/main.cpp
    src/parser.cpp
    src/test_igraph.cpp
    src/graph_builder.cpp
    src/cycle_finder.cpp
    src/currency_service/currency_service.cpp
    src/engine/arbitrage_engine.cpp
    src/simulation/simulation.cpp
    src/rate_service/rate_service.cpp

)


# Link igraph to your executable
target_link_libraries(forex_cycles igraph::igraph)


