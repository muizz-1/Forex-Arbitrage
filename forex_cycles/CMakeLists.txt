cmake_minimum_required(VERSION 3.10)
project(forex_cycles)

set(CMAKE_CXX_STANDARD 17)

find_package(igraph REQUIRED)

add_executable(forex_cycles
    src/main.cpp
    src/engine/arbitrage_engine.cpp    
    src/parser/parser.cpp
    src/graph/graph_builder.cpp 
    src/graph/cycle_finder.cpp
    src/graph/test_igraph.cpp
    src/services/currency_service.cpp
    src/simulation/simulation.cpp

)

target_include_directories(forex_cycles PRIVATE src)

target_link_libraries(forex_cycles igraph::igraph)