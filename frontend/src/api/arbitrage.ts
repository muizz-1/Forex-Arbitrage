import type { ArbitrageRequest, ArbitrageResponse } from "@/types/arbitrage"

const API_URL = "http://127.0.0.1:8000/arbitrage"

export async function detectArbitrage(
  body: ArbitrageRequest
): Promise<ArbitrageResponse> {
  const res = await fetch(API_URL, {
    method: "POST",
    headers: {
      "Content-Type": "application/json",
    },
    body: JSON.stringify(body),
  })

  if (!res.ok) {
    const text = await res.text()
    throw new Error(text)
  }

  return res.json()
}