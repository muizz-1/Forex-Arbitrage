#pragma once

#include <vector>
#include <string>
#include <igraph.h>

struct ArbitrageRequest {
    std::vector<std::string> currencies;
    std::string base_currency;
    bool best_mode;
};

struct ArbitrageResult
{
    bool found = false;

    std::vector<std::string> cycle;
    std::vector<int> cycle_ids;

    double profit_percent = 0.0;
    double total_weight = 0.0;
    igraph_t* graph = nullptr;
    igraph_vector_t* weights = nullptr;
};