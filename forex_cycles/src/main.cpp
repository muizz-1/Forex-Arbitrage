#include "engine/arbitrage_engine.hpp"
#include "currency_service/currency_service.hpp"
#include "simulation/simulation.hpp"

#include <iostream>
#include <thread>
#include <chrono>

using namespace std;

int main()
{
    ArbitrageRequest request;

    // currencies to include
    request.currencies = get_available_currencies();

    // base currency for printing
    request.base_currency = "USD";

    // best cycle mode
    request.best_mode = true;

    double start_amount = 1000.0;
    double tx_cost = 0.001;

    cout << "Starting live arbitrage monitor...\n\n";

    while (true)
    {
        ArbitrageResult result =
            run_arbitrage_engine(request);

        if (!result.found)
        {
            cout << "No arbitrage opportunity detected.\n";
        }
        else
        {
            cout << "Cycle found:\n";

            for (auto &c : result.cycle)
            {
                cout << c << " → ";
            }

            cout << "\nRaw profit: "
                 << result.profit_percent
                 << "%\n";

            // --- simulation ---
            TradeResult sim =
                simulate_trade(
                    result.cycle_ids,   // ids version of cycle
                    start_amount,
                    result.graph,
                    result.weights,
                    tx_cost
                );

            if (sim.valid)
            {
                cout << "Simulation:\n";

                cout << "Start: "
                     << sim.start_amount
                     << endl;

                cout << "End: "
                     << sim.end_amount
                     << endl;

                cout << "Profit: "
                     << sim.profit
                     << endl;

                cout << "Profit %: "
                     << sim.profit_pct
                     << endl;
            }
        }

        cout << "----------------------------------\n";

        this_thread::sleep_for(
            chrono::seconds(5)
        );
    }

    return 0;
}