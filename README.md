# ⚡ HFT Currency Arbitrage Simulator

This project is a **High-Frequency Trading (HFT) Currency Arbitrage Simulator** built to explore how arbitrage opportunities appear in real financial markets, how quickly they disappear, and how difficult it actually is to detect them in real time.

The goal of this project was not just to detect arbitrage, but to simulate the **conditions under which arbitrage actually exists for a few milliseconds**, before markets correct themselves.

This is why the system includes:

* Real forex data (yfinance)
* Artificial noise to simulate temporary mispricing
* A C++ cycle detection engine
* A React/Vite interactive UI
* A FastAPI backend
* A graph-based arbitrage search using negative cycles

---

## 💱 What is Arbitrage?

Arbitrage is the act of taking advantage of price differences in different markets.

Example:

If
USD → EUR → JPY → USD

results in more USD than we started with,
then we found an arbitrage opportunity.

In graph terms:

* Currencies = nodes
* Exchange rates = edges
* Arbitrage = negative cycle (after log transform)

This project detects those cycles automatically.

---

## 📈 What is Interest Rate / Currency Arbitrage?

In real financial markets, arbitrage can come from:

* Different exchange rates across venues
* Different interest rates across countries
* Delayed price updates
* Market microstructure inefficiencies

High-frequency traders exploit **very small and very short-lived mispricings**.

These opportunities may exist for:

* milliseconds
* microseconds
* sometimes even less

By the time retail APIs update, the market has already normalized.

This project tries to simulate the moment **before normalization happens**.

---

## ⚡ Why we intentionally add randomness

We use `yfinance` to fetch forex rates.

The problem:

* yfinance is slow
* it is not a real trading API
* rates arrive already normalized
* not all pairs exist
* data is inconsistent

By the time we receive the rates, arbitrage is already gone.

In real HFT:

* Market updates happen in microseconds
* API latency is milliseconds
* That gap is where arbitrage lives

To simulate real conditions, we intentionally inject small randomness into rates before building the graph.

This creates temporary inefficiencies similar to real markets before they self-correct.

So the simulator shows what could happen in the **tiny window before normalization**.

---

## 🧠 Engine (C++ Arbitrage Core)

The arbitrage engine is written in C++ for speed.

We:

1. Fetch rates using Python
2. Build a directed weighted graph
3. Convert rates using -log(rate)
4. Search for negative cycles using igraph
5. Return cycles to the API

We support:

* Detect first arbitrage cycle
* Detect best arbitrage cycle
* Detect multiple cycles

The engine runs much faster than Python, which is important for graph algorithms.

---

## 🧩 Graph Builder Heuristic

Building a fully connected currency graph is expensive.

If we include every possible edge:

* graph becomes dense
* cycle search becomes slow
* noise increases

So we use a heuristic:

We intentionally skip some positive-weight edges.

Why?

Because arbitrage only exists in negative cycles.

By skipping edges that clearly cannot help form a negative cycle, we:

* reduce graph size
* speed up search
* find arbitrage faster
* simulate HFT-style filtering

This makes the engine closer to real trading systems, where not every edge is considered.

---

## 🖥 Frontend (Vite + React + Graph UI)

Frontend is built using:

* Vite
* React
* Framer Motion
* React Flow

Features:

* Interactive currency graph
* Floating profit panel
* Toggle constraints
* Select currencies
* Choose best cycle vs first cycle
* Visual cycle highlighting

The UI is intentionally designed like a trading dashboard.

---

## 🌐 Backend API (FastAPI)

Backend connects everything:

Frontend → API → C++ engine → API → Frontend

The API may sometimes return no arbitrage.

This can happen because:

* Market is normalized
* yfinance did not return all pairs
* API returned inconsistent data
* Randomness did not create inefficiency
* Graph did not contain a negative cycle

This is realistic.

Real markets also do not always have arbitrage.

And public APIs are not reliable enough for true HFT.

---

## ⚠️ Why the API may not work sometimes

This is expected.

Reasons:

* yfinance is slow
* not all currency pairs exist
* rates may already be normalized
* randomness may not create a cycle
* graph heuristic may remove needed edge

In real trading:

Arbitrage is rare
Arbitrage is short-lived
Arbitrage is hard to detect

This simulator reflects that reality.

---

## 🛠 Tech Stack

Engine

* C++
* igraph

Data

* Python
* yfinance

Backend

* FastAPI

Frontend

* React
* Vite
* Framer Motion
* React Flow

Build

* CMake
* Node
* npm

---

## 🎯 Purpose of this project

This project was built to understand:

* How arbitrage works
* Why HFT systems need to be fast
* Why real markets rarely show arbitrage to public APIs
* How graph algorithms can detect inefficiencies
* How UI + backend + engine can work together

It is not meant to trade.

It is meant to simulate the environment where trading systems operate.

---

## 👤 Author

Built by me as an exploration of

* high-frequency trading
* graph algorithms
* market microstructure
* full-stack systems
* performance engineering

This project is intentionally over-engineered because that is how real trading systems are.
