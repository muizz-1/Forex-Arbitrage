#include "arbitrage_engine.hpp"
#include "../parser.hpp"
#include "../graph_builder.hpp"
#include "../cycle_finder.hpp"
#include <igraph.h>
#include <cmath>
#include <iostream>
#include <algorithm>
#include <tuple>
#include <unordered_map>
#include <vector>
#include <string>
#include <set>
#include "rate_service/rate_service.hpp"

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


ArbitrageResult run_arbitrage_engine(
    const ArbitrageRequest& req)
{
    ArbitrageResult result;

    // -----------------------------
    // 1. fetch raw
    // -----------------------------

    auto raw = fetch_and_parse_from_python();

    // -----------------------------
    // 2. normalize (bid/ask → rates)
    // -----------------------------

    auto rates = normalize_rates(raw);

    // -----------------------------
    // 3. allowed currencies
    // -----------------------------

    std::set<std::string> allowed(
        req.currencies.begin(),
        req.currencies.end()
    );

    // -----------------------------
    // 4. filter
    // -----------------------------

    std::vector<
        std::tuple<
            std::string,
            std::string,
            double
        >
    > filtered_rates;

    for (auto& r : rates)
    {
        const std::string& base = std::get<0>(r);
        const std::string& quote = std::get<1>(r);

        if (allowed.count(base) &&
            allowed.count(quote))
        {
            filtered_rates.push_back(r);
        }
    }

    if (filtered_rates.empty())
        return result;

    // -----------------------------
    // 5. build graph
    // -----------------------------

    std::unordered_map<std::string,int>
        currency_to_id;

    std::vector<std::string>
        id_to_currency;

    igraph_vector_t* weights =
        new igraph_vector_t;

    igraph_t* graph =
        new igraph_t;

    *graph = build_weighted_graph(
        filtered_rates,
        currency_to_id,
        id_to_currency,
        weights
    );

    // -----------------------------
    // 6. find cycle
    // -----------------------------

    auto cycle = find_arbitrage_bellman_ford(
        graph,
        weights,
        id_to_currency.size()
    );

    if (cycle.empty())
        return result;

    // -----------------------------
    // 7. fill result
    // -----------------------------

    result.found = true;
    result.graph = graph;
    result.weights = weights;
    result.cycle_ids = cycle;

    for (int id : cycle)
        result.cycle.push_back(
            id_to_currency[id]
        );

    return result;
}