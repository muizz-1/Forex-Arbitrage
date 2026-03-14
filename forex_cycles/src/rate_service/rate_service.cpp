#include "rate_service.hpp"

#include <random>
#include <cmath>

using namespace std;

static double get_pip_size(
    const string& base,
    const string& quote
)
{
    if (base == "JPY" ||
        quote == "JPY")
        return 0.01;
    return 0.0001;
}

static double random_pips(
    double pip,
    int max_pips = 2
)
{
    static random_device rd;
    static mt19937 gen(rd());

    uniform_int_distribution<> d(
        -max_pips,
        max_pips
    );

    return d(gen) * pip;
}

vector<
    tuple<
        string,
        string,
        double
    >
>
normalize_rates(
    const vector<
        tuple<
            string,
            double,
            double,
            double
        >
    >& raw
)
{
    vector<
        tuple<
            string,
            string,
            double
        >
    > out;

    const int spread_pips = 3;
    const int noise_pips = 2;

    for (auto& r : raw)
    {
        const string& symbol = get<0>(r);
        double last = get<3>(r);

        if (symbol.size() < 6)
            continue;

        if (last <= 0)
            continue;

        string base =
            symbol.substr(0, 3);

        string quote =
            symbol.substr(3, 3);

        double pip =
            get_pip_size(
                base,
                quote
            );

        double spread =
            spread_pips * pip;

        double noise =
            random_pips(
                pip,
                noise_pips
            );

        double mid =
            last + noise;

        if (mid <= 0)
            continue;

        double bid =
            mid - spread / 2.0;

        double ask =
            mid + spread / 2.0;

        if (bid <= 0 ||
            ask <= 0)
            continue;

        // base → quote
        out.emplace_back(
            base,
            quote,
            bid
        );

        // quote → base
        out.emplace_back(
            quote,
            base,
            1.0 / ask
        );
    }

    return out;
}