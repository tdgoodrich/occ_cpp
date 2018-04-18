#include "find_occ.hpp"

bool enum2col   = true;
bool use_gray   = true;
struct bitvec *occ = NULL;
const char **vertices;
unsigned long long augmentations = 0;
struct graph *g;
size_t last_node_finished;
sigset_t signalset;

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
    // last_node_finished. Everything in the range last_node_finished + 1
    // through g->size is unexplored.

    // Print top line of statistics
    // (n, m, OPT, run time (user), flow augmentations)
    printf(
        "%5lu %6lu %5lu %10.2f %16llu\n",
        (unsigned long) g->size,
        (unsigned long) graph_num_edges(g),
        (unsigned long) bitvec_count(occ) + (g->size - last_node_finished - 1),
        user_time(),
        augmentations
    );

    // Print computed OCT set.
    BITVEC_ITER(occ, v) puts(vertices[v]);

    // Print all vertices that have not been explored. We consider these
    // as part of the OCT set.
    for (size_t i = last_node_finished + 1; i < g->size; i++) printf("%s\n", vertices[i]);

    // Now that stats have been printed, finish handling SIGTERM
    // by exiting with success.
    exit(0);

}


void find_occ(const struct graph *g, int preprocessing)
{

    /* Set up the SIGTERM handler */
    struct sigaction action;
    memset(&action, 0, sizeof(struct sigaction));
    sigfillset(&signalset);
    action.sa_handler = term;
    action.sa_mask = signalset;
    sigaction(SIGTERM, &action, NULL);


    // Determine which optimizations to use
    bool run_heuristics = false;
    bool density = false;
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

        // Add all the vertices from the heuristic subgraph to the
        // subgraph used in iterative compression
        for (auto v : heuristic_subgraph) {
            bitvec_set(sub, v);
        }

    }

    /* TODO: go from i=0 to lookup_table size */
    last_node_finished = -1;
    for (size_t i = 0; i < g->size; ++i)
    {

        // Add i to the subgraph we're looking at
	    bitvec_set(sub, i);
	    struct graph *g2 = graph_subgraph(g, sub);

        // If this is already an OCT set, continue on
        block();
        last_node_finished = i;
	    if (occ_is_occ(g2, occ))
        {
            unblock();
	        graph_free(g2);
	        continue;
	    }
        else {
            bitvec_set(occ, i);
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
