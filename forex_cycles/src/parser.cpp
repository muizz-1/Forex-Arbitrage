#include "parser.hpp"
#include <cstdio>
#include <iostream>
#include <sstream>

using namespace std;

vector<tuple<string,double,double,double>>
fetch_and_parse_from_python()
{
    vector<tuple<string,double,double,double>> results;

    FILE* pipe =
        popen("python3 ../data/FetchData.py", "r");

    if (!pipe) {
        cerr << "Failed to run Python\n";
        return results;
    }

    char buffer[1024];
    bool skip_header = false;

    while (fgets(buffer, sizeof(buffer), pipe)) {

        string line(buffer);

        if (!skip_header) {
            skip_header = true;
            continue;
        }

        istringstream ss(line);

        string symbol;
        string bid_str;
        string ask_str;
        string last_str;

        getline(ss, symbol, ',');
        getline(ss, bid_str, ',');
        getline(ss, ask_str, ',');
        getline(ss, last_str, ',');

        if (symbol.size() < 6)
            continue;

        double bid = 0;
        double ask = 0;
        double last = 0;

        try { if (!bid_str.empty()) bid = stod(bid_str); }
        catch (...) {}

        try { if (!ask_str.empty()) ask = stod(ask_str); }
        catch (...) {}

        try { if (!last_str.empty()) last = stod(last_str); }
        catch (...) {}

        results.emplace_back(
            symbol,
            bid,
            ask,
            last
        );
    }

    pclose(pipe);

    return results;
}