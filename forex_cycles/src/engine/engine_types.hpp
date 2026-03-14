#pragma once
#include <vector>
#include <string>

struct ArbitrageRequest {
    std::vector<std::string> currencies;   // currencies selected by user
    std::string base_currency;             // user-selected base
    bool best_mode;                        // false = quick, true = best
    double starting_amount;
    double tx_cost;
};

struct ArbitrageResult
{
    bool found;

    std::vector<std::string> cycle;

    double profit_percent;
    double profit_after_cost;

    double start_amount;
    double end_amount;
    double total_weight;

    std::vector<std::tuple<std::string,std::string,double>> edges;

    std::vector<std::string> nodes;
};