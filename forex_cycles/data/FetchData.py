# #!/usr/bin/env python3

# import yfinance as yf
# import pandas as pd
# import sys
# import itertools
# import random

# def fetch_forex_rates_to_stdout():

#     currencies = [
#         "USD","EUR","JPY","GBP","CAD","NZD",
#         "MXN","RUB","KRW", "COP",  # Colombian Peso
#         "ARS",  # Argentine Peso
#         "NGN",  # Nigerian Naira
#         "KWD",  # Kuwaiti Dinar
#         "QAR",  # Qatari Riyal
        
#     ]

#     base = "USD"

#     # Fetch USD pairs only
#     pairs = [f"{base}{c}=X" for c in currencies if c != base]

#     data = yf.download(
#         tickers=pairs,
#         period="1d",
#         interval="1d",
#         group_by="ticker",
#         progress=True
#     )

#     close_data = data.xs('Close', axis=1, level=1).tail(1)

#     rates = {}

#     for c in currencies:
#         if c == base:
#             rates[c] = 1.0
#             continue

#         pair = f"{base}{c}=X"
#         if pair in close_data.columns:
#             rates[c] = float(close_data[pair])
#         else:
#             rates[c] = None

#     rows = []

#     for a, b in itertools.permutations(currencies, 2):

#         if rates[a] is None or rates[b] is None:
#             continue

#         rate = rates[b] / rates[a]
#         rate *= (1 + random.uniform(-0.00005, 0.00005))
#         rows.append({
#             "symbol": f"{a}{b}",
#             "rate": rate
#         })

#     df = pd.DataFrame(rows)

#     df.to_csv(sys.stdout, index=False)


# if __name__ == "__main__":
#     fetch_forex_rates_to_stdout()




#!/usr/bin/env python3

import yfinance as yf
import sys

currencies = [
    "USD",
    "EUR",
    "JPY",
    "INR",
    "PKR",
    "AED",
    "QAR",
    "MXN",
    "NZD",
    "AUD"
]

pairs = [
    f"{base}{quote}=X"
    for base in currencies
    for quote in currencies
    if base != quote
]
def val(x):
    return "" if x is None else x


def fetch():

    print(f"{p},{bid},{ask},{last}")
    
for p in pairs:

    try:
        t = yf.Ticker(p)

        bid = None
        ask = None
        last = None

        fi = t.fast_info

        bid = fi.get("bid", None)
        ask = fi.get("ask", None)
        last = fi.get("last_price", None)

        def val(x):
            return "" if x is None else x

        print(f"{p},{val(bid)},{val(ask)},{val(last)}")

    except Exception as e:
        continue
if __name__ == "__main__":
    fetch()