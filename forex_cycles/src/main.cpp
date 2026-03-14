#include "engine/arbitrage_engine.hpp"
#include "services/currency_service.hpp"
#include "simulation/simulation.hpp"

#include <iostream>

using namespace std;

void print_json(const ArbitrageResult& r)
{
    cout << "{";

    cout << "\"found\":"
         << (r.found ? "true" : "false");

    cout << ",\"nodes\":[";

    for (size_t i=0;i<r.nodes.size();i++)
    {
        cout << "\"" << r.nodes[i] << "\"";
        if (i+1<r.nodes.size()) cout<<",";
    }

    cout << "]";

    cout << ",\"edges\":[";

    for (size_t i=0;i<r.edges.size();i++)
    {
        auto &e = r.edges[i];

        cout << "{";
        cout << "\"from\":\"" << get<0>(e) << "\",";
        cout << "\"to\":\"" << get<1>(e) << "\",";
        cout << "\"rate\":" << get<2>(e);
        cout << "}";

        if (i+1<r.edges.size()) cout<<",";
    }

    cout << "]";

    if (r.found)
    {
        cout << ",\"cycle\":[";

        for (size_t i=0;i<r.cycle.size();i++)
        {
            cout << "\"" << r.cycle[i] << "\"";
            if (i+1<r.cycle.size()) cout<<",";
        }

        cout << "]";

        cout << ",\"profit_before_cost\":"
             << r.profit_percent;

        cout << ",\"profit_after_cost\":"
             << r.profit_after_cost;

        cout << ",\"start_amount\":"
             << r.start_amount;

        cout << ",\"end_amount\":"
             << r.end_amount;
    }

    cout << "}";
}

#include "engine/arbitrage_engine.hpp"
#include <iostream>
#include <vector>
#include <string>

using namespace std;

void print_json(const ArbitrageResult& r); // keep your existing

int main(int argc, char** argv)
{
    ArbitrageRequest req;

    // defaults
    req.base_currency = "USD";
    req.best_mode = false;
    req.starting_amount = 1000;
    req.tx_cost = 0.00002;

    // -------------------------
    // parse args
    // -------------------------

    for (int i = 1; i < argc; i++)
    {
        string arg = argv[i];

        if (arg == "--base" && i + 1 < argc)
        {
            req.base_currency = argv[++i];
        }
        else if (arg == "--amount" && i + 1 < argc)
        {
            req.starting_amount = stod(argv[++i]);
        }
        else if (arg == "--cost" && i + 1 < argc)
        {
            req.tx_cost = stod(argv[++i]);
        }
        else if (arg == "--best")
        {
            req.best_mode = true;
        }
        else
        {
            // treat as currency
            req.currencies.push_back(arg);
        }
    }

    if (req.currencies.empty())
    {
        cerr << "No currencies provided\n";
        return 1;
    }

    ArbitrageResult r = run_arbitrage_engine(req);

    print_json(r);

    return 0;
}