#include "simulation.hpp"

using namespace std;

TradeResult simulate_trade(
    const vector<string>& cycle,
    double start_amount,
    const vector<
        tuple<string,string,double>
    >& edges,
    double tx_cost
)
{
    double amount = start_amount;

    for (size_t i = 0;
         i < cycle.size() - 1;
         i++)
    {
        const string& from = cycle[i];
        const string& to   = cycle[i + 1];

        double rate = -1;

        for (auto& e : edges)
        {
            if (get<0>(e) == from &&
                get<1>(e) == to)
            {
                rate = get<2>(e);
                break;
            }
        }

        if (rate <= 0)
            continue;

        amount *= rate;

        // transaction cost
        amount *= (1.0 - tx_cost);
    }

    TradeResult r;

    r.start_amount = start_amount;
    r.end_amount = amount;
    r.profit = amount - start_amount;
    r.profit_pct =
        (r.profit / start_amount) * 100.0;

    return r;
}