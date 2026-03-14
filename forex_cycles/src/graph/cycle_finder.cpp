#include <igraph.h>
#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>
#include <iomanip>
#include <cmath>
#include <algorithm>
#include <limits>
#include "../engine/engine_types.hpp"


using namespace std;

void find_arbitrage_cycles(const igraph_t* graph,
                           const igraph_vector_t* weights,
                           const vector<string>& id_to_currency,
                           double threshold = -2) {

                            igraph_vector_int_list_t cycles;
    igraph_vector_int_list_init(&cycles, 0);

    igraph_simple_cycles(graph, &cycles, nullptr, IGRAPH_OUT, 2, igraph_vcount(graph));

    int count = 0;
    for (igraph_integer_t i = 0; i < igraph_vector_int_list_size(&cycles); ++i) {
        const igraph_vector_int_t* cycle = igraph_vector_int_list_get_ptr(&cycles, i);
        double total_weight = 0.0;

        for (igraph_integer_t j = 0; j < igraph_vector_int_size(cycle); ++j) {
            igraph_integer_t from = VECTOR(*cycle)[j];
            igraph_integer_t to = VECTOR(*cycle)[(j + 1) % igraph_vector_int_size(cycle)];

            igraph_integer_t eid;
            igraph_get_eid(graph, &eid, from, to, IGRAPH_DIRECTED, true);
            total_weight += VECTOR(*weights)[eid];
        }

        // cout << (total_weight);
        if (total_weight <= threshold) {
            cout << "Cycle " << (++count) << ": ";
            for (igraph_integer_t j = 0; j < igraph_vector_int_size(cycle); ++j) {
                cout << id_to_currency[VECTOR(*cycle)[j]] << " → ";
            }
            cout << id_to_currency[VECTOR(*cycle)[0]]; // Close the loop
            cout << " | Total weight: " << fixed << setprecision(4) << total_weight << "\n";
        }
    }

    if (count == 0)
        cout << "No arbitrage cycles found.\n";
    else
        cout << "🚨 Found " << count << " arbitrage cycle(s).\n";

    igraph_vector_int_list_destroy(&cycles);
}

void find_best_arbitrage_cycle(const igraph_t* graph,
                               const igraph_vector_t* weights,
                               const vector<string>& id_to_currency) {

    igraph_vector_int_list_t cycles;
    igraph_vector_int_list_init(&cycles, 0);

    igraph_simple_cycles(graph, &cycles, nullptr, IGRAPH_OUT, 2, igraph_vcount(graph));

    double best_weight = 0.0;   // arbitrage requires negative
    vector<int> best_cycle;

    for (igraph_integer_t i = 0; i < igraph_vector_int_list_size(&cycles); ++i) {

        const igraph_vector_int_t* cycle = igraph_vector_int_list_get_ptr(&cycles, i);
        double total_weight = 0.0;

        for (igraph_integer_t j = 0; j < igraph_vector_int_size(cycle); ++j) {

            igraph_integer_t from = VECTOR(*cycle)[j];
            igraph_integer_t to   = VECTOR(*cycle)[(j + 1) % igraph_vector_int_size(cycle)];

            igraph_integer_t eid;
            igraph_get_eid(graph, &eid, from, to, IGRAPH_DIRECTED, true);

            total_weight += VECTOR(*weights)[eid];
        }

        if (total_weight < best_weight) {
            best_weight = total_weight;

            best_cycle.clear();
            for (igraph_integer_t j = 0; j < igraph_vector_int_size(cycle); ++j) {
                best_cycle.push_back(VECTOR(*cycle)[j]);
            }
        }
    }

    if (best_cycle.empty()) {
        cout << "No arbitrage cycle found.\n";
    }
    else {

        cout << "🚨 Best arbitrage cycle:\n";

        for (size_t i = 0; i < best_cycle.size(); ++i) {
            cout << id_to_currency[best_cycle[i]] << " → ";
        }
        cout << id_to_currency[best_cycle[0]];

        double profit = exp(-best_weight) - 1;

        cout << "\nWeight: " << fixed << setprecision(6) << best_weight;
        cout << "\nProfit: " << profit * 100 << "%\n";
    }

    igraph_vector_int_list_destroy(&cycles);
}

ArbitrageResult bellman_ford_arbitrage(const igraph_t* graph,
                                       const igraph_vector_t* weights,
                                       const vector<string>& id_to_currency)
{
    ArbitrageResult result;
    result.found = false;

    int V = igraph_vcount(graph);
    int E = igraph_ecount(graph);

    vector<double> dist(V, 0.0);
    vector<int> parent(V, -1);

    igraph_integer_t from, to;

    for (int i = 0; i < V - 1; i++) {
        for (int eid = 0; eid < E; eid++) {

            igraph_edge(graph, eid, &from, &to);
            double w = VECTOR(*weights)[eid];

            if (dist[from] + w < dist[to]) {
                dist[to] = dist[from] + w;
                parent[to] = from;
            }
        }
    }

    for (int eid = 0; eid < E; eid++) {

        igraph_edge(graph, eid, &from, &to);
        double w = VECTOR(*weights)[eid];

        if (dist[from] + w < dist[to]) {

            int v = to;

            for (int i = 0; i < V; i++)
                v = parent[v];

            vector<int> cycle;
            int start = v;

            do {
                cycle.push_back(v);
                v = parent[v];
            } while (v != start);

            cycle.push_back(start);
            reverse(cycle.begin(), cycle.end());

            double total_weight = 0;

            for (size_t i = 0; i < cycle.size() - 1; i++) {

                igraph_integer_t eid2;
                igraph_get_eid(graph, &eid2, cycle[i], cycle[i+1], IGRAPH_DIRECTED, true);

                total_weight += VECTOR(*weights)[eid2];
            }

            result.found = true;
            result.total_weight = total_weight;
            result.profit_percent = (exp(-total_weight) - 1) * 100;

            for (int node : cycle)
                result.cycle.push_back(id_to_currency[node]);

            return result;
        }
    }

    return result;
}



ArbitrageResult best_arbitrage_cycle(const igraph_t* graph,
                                     const igraph_vector_t* weights,
                                     const vector<string>& id_to_currency)
{
    ArbitrageResult result;
    result.found = false;

    int V = igraph_vcount(graph);
    int E = igraph_ecount(graph);

    double best_weight = numeric_limits<double>::infinity();
    vector<int> best_cycle;

    igraph_integer_t from, to;

    for (int source = 0; source < V; source++) {

        vector<double> dist(V, numeric_limits<double>::infinity());
        vector<int> parent(V, -1);

        dist[source] = 0;

        for (int i = 0; i < V - 1; i++) {
            for (int eid = 0; eid < E; eid++) {

                igraph_edge(graph, eid, &from, &to);
                double w = VECTOR(*weights)[eid];

                if (dist[from] + w < dist[to]) {
                    dist[to] = dist[from] + w;
                    parent[to] = from;
                }
            }
        }

        for (int eid = 0; eid < E; eid++) {

            igraph_edge(graph, eid, &from, &to);
            double w = VECTOR(*weights)[eid];

            if (dist[from] + w < dist[to]) {

                int v = to;

                for (int i = 0; i < V; i++)
                    v = parent[v];

                vector<int> cycle;
                int start = v;

                do {
                    cycle.push_back(v);
                    v = parent[v];
                } while (v != start);

                cycle.push_back(start);
                reverse(cycle.begin(), cycle.end());

                double total_weight = 0;

                for (size_t i = 0; i < cycle.size() - 1; i++) {

                    igraph_integer_t eid2;
                    igraph_get_eid(graph, &eid2, cycle[i], cycle[i+1], IGRAPH_DIRECTED, true);

                    total_weight += VECTOR(*weights)[eid2];
                }

                if (total_weight < best_weight) {
                    best_weight = total_weight;
                    best_cycle = cycle;
                }
            }
        }
    }

    if (best_cycle.empty())
        return result;

    result.found = true;
    result.total_weight = best_weight;
    result.profit_percent = (exp(-best_weight) - 1) * 100;

    for (int node : best_cycle)
        result.cycle.push_back(id_to_currency[node]);

    return result;
}