#!/usr/bin/env python3

import yfinance as yf
import pandas as pd
import sys

def fetch_forex_rates_to_stdout():
    currencies = [
    "USD",  # United States Dollar
    "EUR",  # The Euro 
    "JPY",  # Japanese Yen
    # "GBP",  # Great Britain Pound 
    # "CHF", "AUD", 
    # "CAD", "NZD",
    "CNY", "INR", 
    # "SEK", "NOK", "SGD", "ZAR", "HKD", "BRL",
    # "MXN", "RUB", "KRW", 
    "PKR",
    "AED",
    # "TRY",  # Turkish Lira
    # "PLN",  # Polish Zloty
    # "CZK",  # Czech Koruna
    # "HUF",  # Hungarian Forint
    # "DKK",  # Danish Krone
    "ILS",  # Israeli Shekel
    "SAR",  # Saudi Riyal
    "EGP",  # Egyptian Pound
    # "THB",  # Thai Baht
    # "MYR",  # Malaysian Ringgit
    # "IDR",  # Indonesian Rupiah
    # "PHP",  # Philippine Peso
    # "TWD",  # Taiwan Dollar
    # "VND",  # Vietnamese Dong
    # "CLP",  # Chilean Peso
    "COP",  # Colombian Peso
    "ARS",  # Argentine Peso
    "NGN",  # Nigerian Naira
    "KWD",  # Kuwaiti Dinar
    "QAR",  # Qatari Riyal

]
    pairs = [f"{base}{quote}=X" for base in currencies for quote in currencies if base != quote]

    try:
        data = yf.download(
            tickers=pairs,
            period='1d',
            interval='1d',
            group_by='ticker',
            threads=True,
            progress=True
        )
    except Exception as e:
        print(f"ERROR: Failed to download data: {e}", file=sys.stderr)
        return

    close_data = data.xs('Close', axis=1, level=1).dropna(axis=1, how='all')
    latest = close_data.tail(1).T.dropna().reset_index()
    latest.columns = ['symbol', 'rate']

    # Send CSV-formatted output to stdout
    latest.to_csv(sys.stdout, index=False)

if __name__ == "__main__":
    fetch_forex_rates_to_stdout()
