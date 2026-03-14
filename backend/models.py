from pydantic import BaseModel, Field
from typing import List


class ArbitrageRequest(BaseModel):
    currencies: List[str] = Field(..., min_length=2)
    base_currency: str
    starting_amount: float = Field(..., gt=0)
    tx_cost: float = Field(..., ge=0)
    best_mode: bool = False