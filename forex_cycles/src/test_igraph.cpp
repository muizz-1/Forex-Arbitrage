#include "test_igraph.hpp"
#include <igraph.h>
#include <iostream>

void run_igraph_cycle_test() {
    std::cout << "🔧 Testing igraph_simple_cycles with weights...\n";

    igraph_t graph;
    igraph_vector_int_t edges;
    igraph_vector_int_list_t cycles;
    igraph_vector_t weights;

    // Graph: A(0) → B(1) → C(2) → A(0)
    igraph_vector_int_init(&edges, 6);
    VECTOR(edges)[0] = 0; VECTOR(edges)[1] = 1;
    VECTOR(edges)[2] = 1; VECTOR(edges)[3] = 2;
    VECTOR(edges)[4] = 2; VECTOR(edges)[5] = 0;

    igraph_create(&graph, &edges, 0, IGRAPH_DIRECTED);

    // Add weights
    igraph_vector_init(&weights, 3);
    VECTOR(weights)[0] = 1.5;
    VECTOR(weights)[1] = 2.0;
    VECTOR(weights)[2] = 0.75;

    // Assign edge attribute
    igraph_cattribute_EAN_setv(&graph, "weight", &weights);

    // Find cycles
    igraph_vector_int_list_init(&cycles, 0);
    igraph_simple_cycles(&graph, &cycles, nullptr, IGRAPH_OUT, 2, 10);

    std::cout << "✅ Found " << igraph_vector_int_list_size(&cycles) << " cycle(s):\n";

    for (igraph_integer_t i = 0; i < igraph_vector_int_list_size(&cycles); ++i) {
        const igraph_vector_int_t* cycle = igraph_vector_int_list_get_ptr(&cycles, i);
        std::cout << "Cycle " << i + 1 << ": ";

        double total_weight = 0.0;
        for (igraph_integer_t j = 0; j < igraph_vector_int_size(cycle); ++j) {
            int from = VECTOR(*cycle)[j];
            int to = VECTOR(*cycle)[(j + 1) % igraph_vector_int_size(cycle)];

            std::cout << static_cast<char>('A' + from) << " → ";

            igraph_integer_t eid;
            igraph_get_eid(&graph, &eid, from, to, IGRAPH_DIRECTED, true);
            total_weight += igraph_cattribute_EAN(&graph, "weight", eid);
        }
        std::cout << static_cast<char>('A' + VECTOR(*cycle)[0]);  // Closing the loop
        std::cout << " | Total weight: " << total_weight << "\n";
    }

    igraph_vector_int_list_destroy(&cycles);
    igraph_vector_destroy(&weights);
    igraph_vector_int_destroy(&edges);
    igraph_destroy(&graph);
}
