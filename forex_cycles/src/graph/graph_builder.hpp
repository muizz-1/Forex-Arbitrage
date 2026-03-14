#pragma once

#include <igraph.h>
#include <vector>
#include <string>
#include <tuple>
#include <unordered_map>

using namespace std;
extern vector<
    tuple<string,string,double>
> raw_edges;

igraph_t build_weighted_graph(
    const vector<tuple<string,string,double>>& exchange_data,
    unordered_map<string,int>& currency_to_id,
    vector<string>& id_to_currency,
    igraph_vector_t* weights,
    vector<tuple<string,string,double>>& raw_edges
);
void print_adjacency_matrix(const igraph_t* graph,
                            const igraph_vector_t* weights,
                            const std::vector<std::string>& id_to_currency);