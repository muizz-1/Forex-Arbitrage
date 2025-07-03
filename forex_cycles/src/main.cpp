#include "parser.hpp"
#include "graph_builder.hpp"
#include "cycle_finder.hpp"
#include <iostream>
#include <cstdlib>              // system()
#include <unordered_map>
#include <vector>
#include <string>
#include <tuple>
#include <sstream>


using namespace std;

int main() {
    // 1. Run the Python script to fetch forex rates
    cout << "📡 Running Python fetcher and parsing...\n";
    FILE* pipe = popen("python3 ../data/FetchData.py", "r");
    if (!pipe) {
        cerr << "❌ Failed to run Python script.\n";
        return 1;
    }

    vector<tuple<string, string, double>> exchange_data;
    char buffer[256];
    bool skip_header = true;
    while (fgets(buffer, sizeof(buffer), pipe)) {
        string line(buffer);
        if (skip_header) {
            skip_header = false;
            continue;
        }

        istringstream ss(line);
        string symbol, rate_str;
        if (!getline(ss, symbol, ',')) continue;
        if (!getline(ss, rate_str)) continue;

        try {
            if (symbol.length() < 6) continue;
            string base = symbol.substr(0, 3);
            string quote = symbol.substr(3, 3);
            double rate = stod(rate_str);
            exchange_data.emplace_back(base, quote, rate);
        } catch (...) {
            cerr << "⚠️  Skipping invalid line: " << line;
        }
    }
    pclose(pipe);

    cout << "✅ Parsed " << exchange_data.size() << " exchange rates:\n";

    // 2. Build graph
    unordered_map<string, int> currency_to_id;
    vector<string> id_to_currency;
    igraph_vector_t weights;
    igraph_t graph = build_weighted_graph(exchange_data, currency_to_id, id_to_currency, &weights);
    print_adjacency_matrix(&graph, &weights, id_to_currency);

    // 3. Find arbitrage cycles
    cout << "\n🔍 Searching for arbitrage cycles...\n";
    find_arbitrage_cycles(&graph, &weights, id_to_currency, -0.0015);

    // 4. Cleanup
    igraph_destroy(&graph);
    igraph_vector_destroy(&weights);

    return 0;
}
