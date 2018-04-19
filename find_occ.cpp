#include "find_occ.hpp"

bool enum2col   = true;
bool use_gray   = true;
struct bitvec *occ = NULL;
const char **vertices;
unsigned long long augmentations = 0;
struct graph *g;

size_t last_index_finished = -1;
sigset_t signalset;
bool run_heuristics = false;
bool density = false;
vector<int> remaining_vertices;

double user_time(void) {
    struct tms buf;
    times(&buf);
    return (double) buf.tms_utime / sysconf(_SC_CLK_TCK);
}


// Convenience method for blocking all signals
void block() {
    if (sigprocmask(SIG_BLOCK, &signalset, NULL)) {
        printf("Error in singal block: %d", errno);
    }
}

// Convenience method for unblocking all signals
void unblock() {
    if(sigprocmask(SIG_UNBLOCK, &signalset, NULL)) {
        printf("Error in signal unblock: %d", errno);
    }
}


/* How to respond to a SIGTERM */
void term(int signum)
{

    // Only handle sigterm
    if (signum != SIGTERM) return;

    // We now need to determine the vertices of g that have not been explored
    // and add them to the OCT set. This is determined by the value of
    // last_index_finished. Everything in the range last_index_finished + 1
    // through g->size is unexplored.

    // Print top line of statistics
    // (n, m, OPT, run time (user), flow augmentations)
    printf(
        "%5lu %6lu %5lu %10.2f %16llu\n",
        (unsigned long) g->size,
        (unsigned long) graph_num_edges(g),
        (unsigned long) bitvec_count(occ) +
            (run_heuristics ? remaining_vertices.size() : g->size) - last_index_finished - 1,
        user_time(),
        augmentations
    );

    // Print computed OCT set.
    BITVEC_ITER(occ, v) puts(vertices[v]);

    // Print all vertices that have not been explored. We consider these
    // as part of the OCT set.
    if (run_heuristics) {
        for (size_t i = last_index_finished + 1; i < remaining_vertices.size(); i++) {
             printf("%s\n", vertices[remaining_vertices[i]]);
        }
    }
    else {
        for (size_t i = last_index_finished + 1; i < g->size; i++) printf("%s\n", vertices[i]);
    }

    // Now that stats have been printed, finish handling SIGTERM
    // by exiting with success.
    exit(0);

}


void find_occ(const struct graph *g, int preprocessing, int seed)
{

    /* Set up the SIGTERM handler */
    struct sigaction action;
    memset(&action, 0, sizeof(struct sigaction));
    sigfillset(&signalset);
    action.sa_handler = term;
    action.sa_mask = signalset;
    sigaction(SIGTERM, &action, NULL);


    // Determine which optimizations to use
    if (preprocessing == 1) {
        run_heuristics = true;
    }
    if (preprocessing == 2) {
        run_heuristics = true;
        density = true;
    }

    block();
        occ = bitvec_make(g->size);
    unblock();

    ALLOCA_BITVEC(sub, g->size);

    // If we're running heuristics, compute an initial bipartite subgraph.
    if (run_heuristics) {

        // Create a graph and heuristics ensemble
        Graph heuristics_graph(g->size);
        EnsembleSolver solver;

        // Iterate over all vertices
        for (int v = 0; v < g->size; v++) {

            // Iterate over all neighbors of vertex v
            for (int n = 0; n < g->vertices[v]->deg; n++) {

                // Get the neighbor w
                int w = g->vertices[v]->neighbors[n];

                // If we haven't seen this neighbor yet, add
                // the edge to the heuristics graph.
                if (v < w) heuristics_graph.add_edge(v, w);

            }
        }

        // Run heuristics
        auto heuristic_result = solver.heuristic_solve(heuristics_graph, 250);
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
            for (int i = 0; i < remaining_vertices.size(); i++){

                // Find the index of the vertex after the current index
                // that has the largest number of edges into the
                // heuristic bipartite graph or into a part of the OCT
                // set already considered.
                int max_value = -1;
                int max_idx = -1;
                for (int j = i; j < remaining_vertices.size(); j++) {

                    // Number of edges
                    int num_edges = 0;

                    // Get the current vertex under consideration
                    int v = remaining_vertices[j];

                    // Count the number of edges to the subgraph
                    for (auto w : heuristic_subgraph) {
                        if (heuristics_graph.has_edge(v, w)) num_edges++;
                    }

                    // Count the number of edges to the previously considered vertices
                    for (int k = 0; k < i; k++) {
                        if (heuristics_graph.has_edge(v, remaining_vertices[k])) num_edges++;
                    }

                    // If this vertex has more edges than the max, update
                    if (num_edges > max_value) {
                        max_value = num_edges;
                        max_idx = j;
                    }

                }

                // Swap the values
                int tmp = remaining_vertices[i];
                remaining_vertices[i] = remaining_vertices[max_idx];
                remaining_vertices[max_idx] = tmp;

            }

        }

    }
    else {
        remaining_vertices.resize(g->size);
        iota(remaining_vertices.begin(), remaining_vertices.end(), 0);
    }

    // If we're not using density sorting, then fill the OCT with
    // all vertices in the graph.
    if (!density) {
        std::shuffle(remaining_vertices.begin(), remaining_vertices.end(), std::default_random_engine(seed));
    }

    // Start compression
    for (size_t i = 0; i < remaining_vertices.size(); ++i)
    {

        // Look up vertex
        int v = remaining_vertices[i];

        // Add v to the subgraph we're looking at
	    bitvec_set(sub, v);
	    struct graph *g2 = graph_subgraph(g, sub);

        // If this is already an OCT set, continue on
        block();
        last_index_finished = i;
	    if (occ_is_occ(g2, occ))
        {
            unblock();
	        graph_free(g2);
	        continue;
	    }
        else {
            bitvec_set(occ, v);
            unblock();
        }

	    struct bitvec *occ_new = occ_shrink(g2, occ, enum2col, use_gray, true);

        /* If we cannot compress then occ_new is null and we need to keep occ.
           Otherwise occ should now point to occ_new */
        if (occ_new)
        {
            block();
    	        free(occ);
    	        occ = occ_new;
    	        if (!occ_is_occ(g2, occ))
                {
    		        fprintf(stderr, "Internal error!\n");
    		        abort();
    	        }
            unblock();
	    }
	    graph_free(g2);
    }
}
