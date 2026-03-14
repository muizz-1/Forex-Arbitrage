#include <igraph.h>
#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>
#include "engine/engine_types.hpp"

using namespace std;
std::vector<int>
find_arbitrage_bellman_ford(
    const igraph_t* graph,
    const igraph_vector_t* weights,
    int vertex_count
);



std::vector<int> find_one_arbitrage_cycle(
    const igraph_t* graph,
    const igraph_vector_t* weights,
    const std::vector<std::string>& id_to_currency,
    double threshold
);

ArbitrageResult find_arbitrage_cycles(const igraph_t* graph,
                           const igraph_vector_t* weights,
                           const vector<string>& id_to_currency,
                           double threshold = -0.0015);

ArbitrageResult find_best_arbitrage_cycle(const igraph_t* graph,
                               const igraph_vector_t* weights,
                               const vector<string>& id_to_currency);


ArbitrageResult bellman_ford_arbitrage(const igraph_t* graph,
                            const igraph_vector_t* weights,
                            const vector<string>& id_to_currency);

ArbitrageResult best_arbitrage_cycle(const igraph_t* graph,
                          const igraph_vector_t* weights,
                          const vector<string>& id_to_currency);