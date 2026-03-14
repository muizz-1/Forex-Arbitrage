#pragma once

#include <vector>
#include <tuple>
#include <string>

std::vector<
    std::tuple<
        std::string,
        std::string,
        double
    >
>
normalize_rates(
    const std::vector<
        std::tuple<
            std::string,
            double,
            double,
            double
        >
    >& raw
);