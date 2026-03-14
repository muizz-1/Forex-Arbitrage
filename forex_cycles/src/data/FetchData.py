#!/usr/bin/env python3

import yfinance as yf
import pandas as pd
import sys
import itertools
import random


def get_currencies():

    # currencies passed from C++
    if len(sys.argv) > 1:
        return sys.argv[1:]

    # fallback (for manual run)
    return [
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
        "CZK",
        "TRY",
        "ZAR",
        "HUF"
    ]


def fetch_forex_rates_to_stdout():

    currencies = get_currencies()

    if len(currencies) < 2:
        return

    # choose base automatically
    base = currencies[0]

    # fetch only base pairs
    pairs = [
        f"{base}{c}=X"
        for c in currencies
        if c != base
    ]

    data = yf.download(
        tickers=pairs,
        period="1d",
        interval="1d",
        group_by="ticker",
        progress=False
    )

    close_data = data.xs(
        "Close",
        axis=1,
        level=1
    ).tail(1)

    rates = {}

    for c in currencies:

        if c == base:
            rates[c] = 1.0
            continue

        pair = f"{base}{c}=X"

        if pair in close_data.columns:
            try:
                rates[c] = float(
                    close_data[pair].iloc[0]
                )
            except:
                rates[c] = None
        else:
            rates[c] = None

    rows = []

    for a, b in itertools.permutations(
        currencies, 2
    ):

        if rates[a] is None:
            continue

        if rates[b] is None:
            continue

        rate = rates[b] / rates[a]

        # noise for demo
        rate *= (
            1 + random.uniform(
                -0.0005,
                0.0005
            )
        )

        rows.append({
            "symbol": f"{a}{b}",
            "rate": rate
        })

    df = pd.DataFrame(rows)

    df.to_csv(
        sys.stdout,
        index=False
    )


if __name__ == "__main__":
    fetch_forex_rates_to_stdout()