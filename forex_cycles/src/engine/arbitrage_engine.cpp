#include "../engine/arbitrage_engine.hpp"
#include "../parser/parser.hpp"
#include "../graph/graph_builder.hpp"
#include "../graph/cycle_finder.hpp"
#include "../simulation/simulation.hpp"
#include <igraph.h>
#include <cmath>
#include <iostream>
#include <algorithm>
#include <tuple>
#include <unordered_map>
#include <vector>
#include <string>

using namespace std;

bool contains(const vector<string>& list, const string& item)
{
    return find(list.begin(), list.end(), item) != list.end();
}

void rotate_cycle_to_base(vector<string>& cycle, const string& base)
{
    if (cycle.empty()) return;

    // If cycle is closed like USD EUR JPY USD, remove last duplicate temporarily
    bool closed = (cycle.size() > 1 && cycle.front() == cycle.back());
    if (closed) {
        cycle.pop_back();
    }

    auto it = find(cycle.begin(), cycle.end(), base);
    if (it == cycle.end()) {
        if (closed) cycle.push_back(cycle.front());
        return;
    }

    rotate(cycle.begin(), it, cycle.end());

    // Close the cycle again
    cycle.push_back(cycle.front());
}

ArbitrageResult run_arbitrage_engine(const ArbitrageRequest& request)
{
    ArbitrageResult result{};
    result.found = false;
    result.total_weight = 0.0;
    result.profit_percent = 0.0;

    auto exchange_data = fetch_and_parse_from_python(
        request.currencies
    ); 
    if (exchange_data.empty()) {
        cerr << "No exchange data received\n";
        return result;
    }

    vector<tuple<string, string, double>> filtered_rates;

    for (const auto& r : exchange_data) {
        const string& base = get<0>(r);
        const string& quote = get<1>(r);

        if (contains(request.currencies, base) &&
            contains(request.currencies, quote)) {
            filtered_rates.push_back(r);
        }
    }

    if (filtered_rates.empty()) {
        cerr << "No rates after filtering currencies\n";
        return result;
    }

    unordered_map<string, int> currency_to_id;
    vector<string> id_to_currency;
    igraph_vector_t weights;

    raw_edges.clear();

    igraph_t graph = build_weighted_graph(
        filtered_rates,
        currency_to_id,
        id_to_currency,
        &weights,
        raw_edges
    );

    

    if (request.best_mode)
        result = best_arbitrage_cycle(&graph, &weights, id_to_currency);
    else
        result = bellman_ford_arbitrage(&graph, &weights, id_to_currency);

    result.found = false;
    result.edges = raw_edges;
    result.nodes = id_to_currency;

    TradeResult trade_result = simulate_trade (
        result.cycle,
        request.starting_amount,      
        result.edges,
        request.tx_cost  
    );

    result.end_amount = trade_result.end_amount;
    result.start_amount = request.starting_amount;
    result.profit_percent; 
    result.profit_after_cost = trade_result.profit_pct;
    
    if (trade_result.profit_pct > 0)
        result.found = true;
        rotate_cycle_to_base(result.cycle, request.base_currency);

    igraph_destroy(&graph);
    igraph_vector_destroy(&weights);

    return result;
}