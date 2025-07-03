#include "parser.hpp"
#include <cstdio>
#include <iostream>
#include <sstream>

using namespace std;

vector<tuple<string, string, double>> fetch_and_parse_from_python() {
    vector<tuple<string, string, double>> results;

    FILE* pipe = popen("python3 ../data/FetchData.py", "r");
    if (!pipe) {
        cerr << "Failed to run Python script.\n";
        return results;
    }

    char buffer[1024];
    bool header_skipped = false;

    while (fgets(buffer, sizeof(buffer), pipe)) {
        string line(buffer);
        if (!header_skipped) {
            header_skipped = true;
            continue; // Skip "symbol,rate"
        }

        istringstream ss(line);
        string symbol, rate_str;
        if (!std::getline(ss, symbol, ',')) continue;
        if (!std::getline(ss, rate_str)) continue;

        if (symbol.length() < 6 || rate_str.empty()) continue;

        // Try parsing the rate safely
        try {
            double rate = std::stod(rate_str);
            std::string base = symbol.substr(0, 3);
            std::string quote = symbol.substr(3, 3);
            results.emplace_back(base, quote, rate);
        } catch (const std::exception& e) {
            std::cerr << "⚠️  Skipping invalid line: " << line << " | Reason: " << e.what() << "\n";
            continue;
        }

    }

    pclose(pipe);
    return results;
}
