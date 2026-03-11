cmake_minimum_required(VERSION 3.10)
project(forex_cycles)

set(CMAKE_CXX_STANDARD 17)

# Find igraph if it was installed globally (in /usr/local)
find_package(igraph REQUIRED)

add_executable(forex_cycles
    src/main.cpp
    src/parser.cpp
    src/test_igraph.cpp
    src/graph_builder.cpp
    src/cycle_finder.cpp
    src/services/currency_service.cpp
    src/engine/arbitrage_engine.cpp
)


# Link igraph to your executable
target_link_libraries(forex_cycles igraph::igraph)


