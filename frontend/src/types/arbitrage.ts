export type ArbitrageResponse = {
  found: boolean

  nodes: string[]

  edges: {
    from: string
    to: string
    rate: number
  }[]

  cycle: string[]

  profit_before_cost: number
  profit_after_cost: number

  start_amount: number
  end_amount: number
}


export type ArbitrageRequest = {
  currencies: string[]
  base_currency: string
  starting_amount: number
  tx_cost: number
  best_mode: boolean
}