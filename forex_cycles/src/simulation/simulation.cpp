#include "simulation.hpp"
#include <cmath>
#include <iostream>

TradeResult simulate_trade(
    const std::vector<int>& cycle,
    double start_amount,
    const igraph_t* graph,
    const igraph_vector_t* weights,
    double tx_cost
) {
    TradeResult result;

    result.valid = false;
    result.start_amount = start_amount;
    result.end_amount = start_amount;
    result.profit = 0;
    result.profit_pct = 0;

    if (cycle.size() < 2) {
        return result;
    }

    double amount = start_amount;

    for (size_t i = 0; i < cycle.size(); i++) {

        int from = cycle[i];
        int to = cycle[(i + 1) % cycle.size()];

        igraph_integer_t eid;

        if (igraph_get_eid(
                graph,
                &eid,
                from,
                to,
                IGRAPH_DIRECTED,
                true) != 0) {
            return result;
        }

        double weight = VECTOR(*weights)[eid];

        // recover rate
        double rate = std::exp(-weight);

        // apply trade
        amount *= rate;

        // apply transaction cost
        if (tx_cost > 0) {
            amount *= (1.0 - tx_cost);
        }
    }

    result.valid = true;
    result.end_amount = amount;
    result.profit = amount - start_amount;

    if (start_amount > 0) {
        result.profit_pct =
            (result.profit / start_amount) * 100.0;
    }

    return result;
}