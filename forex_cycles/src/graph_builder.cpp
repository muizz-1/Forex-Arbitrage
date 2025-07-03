#include "graph_builder.hpp"
#include <cmath>
#include <iostream>
#include <iomanip>  // for std::setw
#include <limits>   // for std::numeric_limits

using namespace std;

igraph_t build_weighted_graph(
    const vector<tuple<string, string, double>>& exchange_data,
    unordered_map<string, int>& currency_to_id,
    vector<string>& id_to_currency,
    igraph_vector_t* weights) {

    vector<igraph_integer_t> edge_list;
    vector<double> weight_list;
    int next_id = 0;

    for (const auto& entry : exchange_data) {
        string base, quote;
        double rate;
        tie(base, quote, rate) = entry;

        if (rate <= 0) {
           continue;
        } 

        if (currency_to_id.find(base) == currency_to_id.end()) {
            currency_to_id[base] = next_id++;
            id_to_currency.push_back(base);
        }
        if (currency_to_id.find(quote) == currency_to_id.end()) {
            currency_to_id[quote] = next_id++;
            id_to_currency.push_back(quote);
        }

        int from = currency_to_id[base];
        int to = currency_to_id[quote];
        double weight = -log(rate);

        if (weight > 5.00) {
            continue;
        }

        edge_list.push_back(from);
        edge_list.push_back(to);
        weight_list.push_back(weight);
    }

    igraph_t graph;
    igraph_vector_int_t edges;
    igraph_vector_int_view(&edges, edge_list.data(), edge_list.size());

    igraph_create(&graph, &edges, next_id, IGRAPH_DIRECTED);
    igraph_vector_init(weights, weight_list.size());
    
    for (size_t i = 0; i < weight_list.size(); ++i) {
        VECTOR(*weights)[i] = weight_list[i];
    }

    return graph;
}



void print_adjacency_matrix(const igraph_t* graph,
                            const igraph_vector_t* weights,
                            const std::vector<std::string>& id_to_currency) {
    igraph_integer_t vcount = igraph_vcount(graph);
    igraph_integer_t ecount = igraph_ecount(graph);

    // Build a 2D matrix initialized with INF
    std::vector<std::vector<double>> matrix(vcount, std::vector<double>(vcount, std::numeric_limits<double>::infinity()));

    for (igraph_integer_t eid = 0; eid < ecount; ++eid) {
        igraph_integer_t from, to;
        igraph_edge(graph, eid, &from, &to);
        matrix[from][to] = VECTOR(*weights)[eid];
    }

    // Print header row
    std::cout << std::setw(6) << " ";
    for (int j = 0; j < vcount; ++j)
        std::cout << std::setw(8) << id_to_currency[j];
    std::cout << "\n";

    // Print rows
    for (int i = 0; i < vcount; ++i) {
        std::cout << std::setw(6) << id_to_currency[i];
        for (int j = 0; j < vcount; ++j) {
            if (std::isinf(matrix[i][j]))
                std::cout << std::setw(8) << "INF";
            else
                std::cout << std::setw(8) << std::fixed << std::setprecision(2) << matrix[i][j];
        }
        std::cout << "\n";
    }
}
