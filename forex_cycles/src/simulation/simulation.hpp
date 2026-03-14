#pragma once

#include <igraph.h>
#include <vector>
#include <string>

struct TradeResult {
    bool valid;
    double start_amount;
    double end_amount;
    double profit;
    double profit_pct;
};

TradeResult simulate_trade(
    const std::vector<int>& cycle,
    double start_amount,
    const igraph_t* graph,
    const igraph_vector_t* weights,
    double tx_cost
);