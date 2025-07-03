#include <igraph.h>
#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>

using namespace std;

void find_arbitrage_cycles(const igraph_t* graph,
                           const igraph_vector_t* weights,
                           const vector<string>& id_to_currency,
                           double threshold = -0.0001) {

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
            cout << " | Total weight: " << total_weight << "\n";
        }
    }

    if (count == 0)
        cout << "No arbitrage cycles found.\n";
    else
        cout << "🚨 Found " << count << " arbitrage cycle(s).\n";

    igraph_vector_int_list_destroy(&cycles);
}
