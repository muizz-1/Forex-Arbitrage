#include "parser.hpp"
#include <cstdio>
#include <iostream>
#include <sstream>

using namespace std;

vector<tuple<string,string,double>>
fetch_and_parse_from_python(
    const vector<string>& currencies
)
{
    vector<tuple<string,string,double>> exchange_data;

    string cmd = "python3 src/data/FetchData.py";

    for (auto& c : currencies)
    {
        cmd += " " + c;
    }

    FILE* pipe = popen(cmd.c_str(), "r");

    if (!pipe) {
        cerr << "Failed to run python\n";
        return exchange_data;
    }

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

            string base = symbol.substr(0,3);
            string quote = symbol.substr(3,3);

            double rate = stod(rate_str);

            exchange_data.emplace_back(
                base,
                quote,
                rate
            );
        }
        catch (...) {}
    }

    pclose(pipe);

    return exchange_data;
}