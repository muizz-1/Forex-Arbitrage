#include <igraph.h>
#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>

using namespace std;

void find_arbitrage_cycles(const igraph_t* graph,
                           const igraph_vector_t* weights,
                           const vector<string>& id_to_currency,
                           double threshold = -0.0015);