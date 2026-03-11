// #include "services/currency_service.hpp"
// #include "engine/arbitrage_engine.hpp"
// #include "parser.hpp"
// #include "graph_builder.hpp"
// #include "cycle_finder.hpp"
// #include <iostream>
// #include <cstdlib>              // system()
// #include <unordered_map>
// #include <vector>
// #include <string>
// #include <tuple>
// #include <sstream>

// using namespace std;

// // int main() {

// //     auto currencies = get_available_currencies();

// //     std::cout << "Available currencies:\n";

// //     for (auto& c : currencies) {
// //         std::cout << c << "\n";
// //     }    

// //     // 1. Run the Python script to fetch forex rates
// //     cout << "📡 Running Python fetcher and parsing...\n";
// //     FILE* pipe = popen("python3 ../data/FetchData.py", "r");
// //     if (!pipe) {
// //         cerr << "❌ Failed to run Python script.\n";
// //         return 1;
// //     }

// //     vector<tuple<string, string, double>> exchange_data;
// //     char buffer[256];
// //     bool skip_header = true;
// //     while (fgets(buffer, sizeof(buffer), pipe)) {
// //         string line(buffer);
// //         if (skip_header) {
// //             skip_header = false;
// //             continue;
// //         }

// //         istringstream ss(line);
// //         string symbol, rate_str;
// //         if (!getline(ss, symbol, ',')) continue;
// //         if (!getline(ss, rate_str)) continue;

// //         try {
// //             if (symbol.length() < 6) continue;
// //             string base = symbol.substr(0, 3);
// //             string quote = symbol.substr(3, 3);
// //             double rate = stod(rate_str);
// //             exchange_data.emplace_back(base, quote, rate);
// //         } catch (...) {
// //             cerr << "⚠️  Skipping invalid line: " << line;
// //         }
// //     }
// //     pclose(pipe);

// //     cout << "✅ Parsed " << exchange_data.size() << " exchange rates:\n";

// //     // 2. Build graph
// //     unordered_map<string, int> currency_to_id;
// //     vector<string> id_to_currency;
// //     igraph_vector_t weights;
// //     igraph_t graph = build_weighted_graph(exchange_data, currency_to_id, id_to_currency, &weights);
// //     print_adjacency_matrix(&graph, &weights, id_to_currency);

// //     // 3. Find arbitrage cycles
// //     cout << "\n🔍 Searching for arbitrage cycles...\n";
// //     best_arbitrage_cycle(&graph, &weights, id_to_currency);

// //     // 4. Cleanup
// //     igraph_destroy(&graph);
// //     igraph_vector_destroy(&weights);

// //     return 0;
// // }


// int main() {

//     ArbitrageRequest req;

//     req.currencies = {"USD","EUR","JPY","GBP"};
//     req.base_currency = "USD";
//     req.best_mode = true;

//     ArbitrageResult res = run_arbitrage_engine(req);

//     if (!res.found) {
//         std::cout << "No arbitrage found\n";
//         return 0;
//     }

//     std::cout << "Best cycle:\n";

//     for (auto& c : res.cycle)
//         std::cout << c << " → ";

//     std::cout << "\nProfit: " << res.profit_percent << "%\n";

//     req.best_mode = false;

//     // ArbitrageResult res2 = run_arbitrage_engine(req);

//     // if (!res2.found) {
//     //     std::cout << "No arbitrage found\n";
//     //     return 0;
//     // }

//     // std::cout << "cycle:\n";

//     // for (auto& c : res2.cycle)
//     //     std::cout << c << " → ";

//     // std::cout << "\nProfit: " << res2.profit_percent << "%\n";

// }


#include "engine/arbitrage_engine.hpp"
#include "services/currency_service.hpp"

#include <iostream>
#include <thread>
#include <chrono>

using namespace std;

int main()
{
    ArbitrageRequest request;

    // currencies user wants to include
    request.currencies = get_available_currencies();

    // base currency for displaying cycles
    request.base_currency = "USD";

    // true = most profitable cycle
    // false = first arbitrage detected
    request.best_mode = true;

    cout << "Starting live arbitrage monitor...\n\n";

    while (true)
    {
        ArbitrageResult result = run_arbitrage_engine(request);

        if (!result.found)
        {
            cout << "No arbitrage opportunity detected.\n";
        }
        else
        {
            cout << "🚨 Arbitrage Opportunity!\n";

            for (size_t i = 0; i < result.cycle.size(); i++)
            {
                cout << result.cycle[i];

                if (i != result.cycle.size() - 1)
                    cout << " → ";
            }

            cout << "\nProfit: " << result.profit_percent << "%\n";
        }

        cout << "----------------------------------\n";

        // wait before next scan
        this_thread::sleep_for(chrono::seconds(5));
    }

    return 0;
}