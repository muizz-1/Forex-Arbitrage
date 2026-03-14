#pragma once

#include <vector>
#include <tuple>
#include <string>

struct TradeResult
{
    double start_amount;
    double end_amount;
    double profit;
    double profit_pct;
};

TradeResult simulate_trade(
    const std::vector<std::string>& cycle,
    double start_amount,
    const std::vector<
        std::tuple<std :: string, std :: string,double>
    >& edges,
    double tx_cost
);