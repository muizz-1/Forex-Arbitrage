from fastapi import FastAPI
from models import ArbitrageRequest
from engine_runner import run_engine
from pprint import pprint
app = FastAPI()


@app.get("/")
def root():
    return {"message": "Backend running"}


@app.post("/arbitrage")
def arbitrage(req: ArbitrageRequest):
    return run_engine(req)


if __name__ == "__main__":

    req = ArbitrageRequest(
        currencies=["USD", "EUR", "PKR", "INR", "HKD", "CZK", "MXN"],
        base_currency="USD",
        starting_amount=1000,
        tx_cost=0.00002,
        best_mode=True,
    )

    result = run_engine(req)

    pprint(result)