#include "currency_service.hpp"

std::vector<std::string> get_available_currencies() {
    return {
        "USD",
        "EUR",
        "JPY",
        "GBP",
        "CAD",
        "NZD",
        "MXN",
        "RUB",
        "KRW",
        "AED",
        "SAR",
        "PKR",
        "INR", 
        "DKK",
        "HKD",
        "CZK"
    };
}