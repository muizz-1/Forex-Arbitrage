#!/usr/bin/env python3

import yfinance as yf
import pandas as pd
import sys
import itertools
import random

def fetch_forex_rates_to_stdout():

    currencies = [
        "USD","EUR","JPY","GBP","CAD","NZD",
        "MXN","RUB","KRW", "COP",  # Colombian Peso
        "ARS",  # Argentine Peso
        "NGN",  # Nigerian Naira
        "KWD",  # Kuwaiti Dinar
        "QAR",  # Qatari Riyal
    ]

    base = "USD"

    # Fetch USD pairs only
    pairs = [f"{base}{c}=X" for c in currencies if c != base]

    data = yf.download(
        tickers=pairs,
        period="1d",
        interval="1d",
        group_by="ticker",
        progress=True
    )

    close_data = data.xs('Close', axis=1, level=1).tail(1)

    rates = {}

    for c in currencies:
        if c == base:
            rates[c] = 1.0
            continue

        pair = f"{base}{c}=X"
        if pair in close_data.columns:
            rates[c] = float(close_data[pair])
        else:
            rates[c] = None

    rows = []

    for a, b in itertools.permutations(currencies, 2):

        if rates[a] is None or rates[b] is None:
            continue

        rate = rates[b] / rates[a]
        rate *= (1 + random.uniform(-0.0005, 0.0005))
        rows.append({
            "symbol": f"{a}{b}",
            "rate": rate
        })

    df = pd.DataFrame(rows)

    df.to_csv(sys.stdout, index=False)


if __name__ == "__main__":
    fetch_forex_rates_to_stdout()