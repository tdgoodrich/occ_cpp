/* occ -- find minimum odd cycle covers (Graph Bipartization)
   Copyright (C) 2006 Falk Hueffner

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License as
   published by the Free Software Foundation; either version 2 of
   the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with GCC; see the file COPYING.  If not, write to the Free
   Software Foundation, 51 Franklin Street, Fifth Floor, Boston, MA
   02110-1301, USA.  */

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>


#if defined (__SVR4) && defined (__sun)
int getopt(int argc, const char *argv[], const char *optstring);
#else
# include <getopt.h>
#endif
#include <unistd.h>
#include <sys/times.h>

/* Handle SIGTERM */
#include <signal.h>

#include "bitvec.h"
#include "graph.h"
#include "occ.h"


double user_time(void) {
    struct tms buf;
    times(&buf);
    return (double) buf.tms_utime / sysconf(_SC_CLK_TCK);
}


void usage(FILE *stream) {
    fprintf(stream,
	    "occ: Calculate minimum odd cycle cover\n"
	    "  -v  Print progress to stderr\n"
        "  -f  Compute OCT on this graph file"
	    "  -h  Display this list of options\n"
        "  -p  Preprocessing level {0: None, 1: Bipartite, 2: Bipartite + Density Sort}\n"
	);
}

/* Always use enum2col */
bool enum2col   = true;
bool use_gray   = true;

/* The actual parameters */
bool verbose    = false;
char *graph_filename;
int preprocessing_level = 0;
unsigned long long augmentations = 0;

/* Move items printed by SIGTERM handler to be global vars */
struct bitvec *occ = NULL;
const char **vertices;
struct graph *g;
size_t last_node_finished;
sigset_t signalset;


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


void find_occ(const struct graph *g)
{
    verbose = false;

    block();
        occ = bitvec_make(g->size);
    unblock();

    ALLOCA_BITVEC(sub, g->size);

    /* TODO: Start with our bipartite subgraph

    for vertex in heuristic_bipartite_nodes:
                bitvec_set(sub, vertex);
    */

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

        // Otherwise add i to the OCT set and compress
	    if (verbose)
        {
	        fprintf(stderr, "size = %3lu ", (unsigned long) graph_num_vertices(g2));
	        fprintf(stderr, "occ = ");
	        bitvec_dump(occ);
	        putc('\n', stderr);
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


int main(int argc, char *argv[]) {
    /* Set up the SIGTERM handler */
    struct sigaction action;
    memset(&action, 0, sizeof(struct sigaction));
    sigfillset(&signalset);
    action.sa_handler = term;
    action.sa_mask = signalset;
    sigaction(SIGTERM, &action, NULL);

    int c;
    while ((c = getopt(argc, argv, "vhf:p:")) != -1)
    {
	    switch (c)
        {
	        case 'v': verbose    = true; break;
	        case 'h': usage(stdout); exit(0); break;
            case 'f': graph_filename = optarg; break;
            case 'p': preprocessing_level = atoi(optarg); break;
	        default:  usage(stderr); exit(1); break;
	    }
    }

    FILE *graph_stream = fopen(graph_filename, "r");
    if (graph_stream == NULL) {
        fprintf(stderr, "File '%s' could not be read.\n", graph_filename);
        exit(1);
    }
    g = graph_read(graph_stream, &vertices);
    fclose(graph_stream);

    size_t occ_size;

    /* Populate global var occ with the OCT set */
    find_occ(g);

    occ_size = bitvec_count(occ);

    printf("%5lu %6lu %5lu %10.2f %16llu\n",
           (unsigned long) g->size, (unsigned long) graph_num_edges(g),
           (unsigned long) occ_size, user_time(), augmentations);
    BITVEC_ITER(occ, v) puts(vertices[v]);

    return 0;
}
