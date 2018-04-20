#include "find_occ.hpp"

bool enum2col   = true;
bool use_gray   = true;
struct bitvec *occ = NULL;
const char **vertices;
unsigned long long augmentations = 0;
struct graph *g;

size_t last_index_finished = -1;
bool run_heuristics = false;
bool density = false;
vector<int> remaining_vertices;


double user_time(void) {
    struct tms buf;
    times(&buf);
    return (double) buf.tms_utime / sysconf(_SC_CLK_TCK);
}


void find_occ(const struct graph *g, int preprocessing, int seed, long htime)
{

    // Determine which optimizations to use
    if (preprocessing == 1) {
        run_heuristics = true;
    }
    if (preprocessing == 2) {
        run_heuristics = true;
        density = true;
    }

    // Make the occ and subgraph bitvectors
    occ = bitvec_make(g->size);
    ALLOCA_BITVEC(sub, g->size);

    // If we're running heuristics, compute an initial bipartite subgraph.
    if (run_heuristics) {

        // Create a graph and heuristics ensemble
        Graph heuristics_graph(g->size);
        EnsembleSolver solver;

        // Iterate over all vertices
        for (size_t v = 0; v < g->size; v++) {

            // Iterate over all neighbors of vertex v
            for (size_t n = 0; n < g->vertices[v]->deg; n++) {

                // Get the neighbor w
                size_t w = g->vertices[v]->neighbors[n];

                // If we haven't seen this neighbor yet, add
                // the edge to the heuristics graph.
                if (v < w) heuristics_graph.add_edge(v, w);

            }
        }

        // Run heuristics
        auto heuristic_result = solver.heuristic_solve(heuristics_graph, received_timeout, htime, seed);
        auto heuristic_subgraph = get<0>(heuristic_result);

        // We save heuristic oct globally so it can be used by the SIGTERM handler
        remaining_vertices = get<1>(heuristic_result);

        // Add all the vertices from the heuristic subgraph to the
        // subgraph used in iterative compression
        for (auto v : heuristic_subgraph) {
            bitvec_set(sub, v);
        }

        // If we're using density, we want to order the vertices in
        // the oct set so that we iterate over them correctly.
        if (density) {

            // Iterate over all the indices of remaining_vertices
            for (size_t i = 0; i < remaining_vertices.size(); i++){

                // Find the index of the vertex after the current index
                // that has the largest number of edges into the
                // heuristic bipartite graph or into a part of the OCT
                // set already considered.
                int max_value = -1;
                int max_idx = -1;
                for (size_t j = i; j < remaining_vertices.size(); j++) {

                    // Number of edges
                    int num_edges = 0;

                    // Get the current vertex under consideration
                    size_t v = remaining_vertices[j];

                    // Count the number of edges to the subgraph
                    for (auto w : heuristic_subgraph) {
                        if (heuristics_graph.has_edge(v, w)) num_edges++;
                    }

                    // Count the number of edges to the previously considered vertices
                    for (size_t k = 0; k < i; k++) {
                        if (heuristics_graph.has_edge(v, remaining_vertices[k])) num_edges++;
                    }

                    // If this vertex has more edges than the max, update
                    if (num_edges > max_value) {
                        max_value = num_edges;
                        max_idx = j;
                    }

                }

                // Swap the values
                size_t tmp = remaining_vertices[i];
                remaining_vertices[i] = remaining_vertices[max_idx];
                remaining_vertices[max_idx] = tmp;

            }

        }

    }
    else {
        remaining_vertices.resize(g->size);
        iota(remaining_vertices.begin(), remaining_vertices.end(), 0);
    }

    // If we're not using density sorting, then randomly shuffle remaining_vertices
    if (!density) {
        std::shuffle(remaining_vertices.begin(), remaining_vertices.end(), std::default_random_engine(seed));
    }

    // Start compression
    size_t i = 0;
    while (!received_timeout && i < remaining_vertices.size()) {

        // Look up vertex
        int v = remaining_vertices[i];

        // Add v to the subgraph we're looking at
	    bitvec_set(sub, v);
	    struct graph *g2 = graph_subgraph(g, sub);

        // If this is already an OCT set, continue on
        last_index_finished = i;
	    if (occ_is_occ(g2, occ))
        {
            // Make sure to increment loop control and free
            // subgraph before continuing.
            i++;
	        graph_free(g2);
	        continue;
	    }
        else {
            bitvec_set(occ, v);
        }

	    struct bitvec *occ_new = occ_shrink(g2, occ, enum2col, use_gray, true, &received_timeout);

        /* If we cannot compress then occ_new is null and we need to keep occ.
           Otherwise occ should now point to occ_new */
        if (occ_new)
        {
	        free(occ);
	        occ = occ_new;
	        if (!occ_is_occ(g2, occ))
            {
		        fprintf(stderr, "Internal error!\n");
		        abort();
	        }
	    }

        // Free the subgraph
	    graph_free(g2);

        // Increment i
        i++;

    }

    // Print top line of statistics
    // (n, m, OPT, run time (user), flow augmentations)
    printf(
        "%5lu %6lu %5lu %10.2f %16llu\n",
        (unsigned long) g->size,
        (unsigned long) graph_num_edges(g),
        (unsigned long) bitvec_count(occ) + remaining_vertices.size() - last_index_finished - 1,
        user_time(),
        augmentations
    );

    // Print computed OCT set.
    BITVEC_ITER(occ, v) puts(vertices[v]);

    // Print all vertices that have not been explored. We consider these
    // as part of the OCT set.
    for (size_t i = last_index_finished + 1; i < remaining_vertices.size(); i++) {
         printf("%s\n", vertices[remaining_vertices[i]]);
    }

}
