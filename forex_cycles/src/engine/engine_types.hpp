#pragma once
#include <vector>
#include <string>

struct ArbitrageRequest {
    std::vector<std::string> currencies;   // currencies selected by user
    std::string base_currency;             // user-selected base
    bool best_mode;                        // false = quick, true = best
};

struct ArbitrageResult {
    bool found;
    std::vector<std::string> cycle;
    double total_weight;
    double profit_percent;
};