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
	    "  -b  Enumerate valid partitions only for bipartite subgraphs\n"
	    "  -g  Enumerate valid partitions by gray code\n"
	    "  -v  Print progress to stderr\n"
	    "  -h  Display this list of options\n"
	);
}

bool enum2col   = false;
bool use_gray   = false;
bool verbose    = false;
bool stats_only = false;
unsigned long long augmentations = 0;

/* Move items printed by SIGTERM handler to be global vars */
struct bitvec *occ = NULL;
const char **vertices;


/* How to respond to a SIGTERM */
void term(int signum)
{
    /* TODO: A temp solution: Print the OCT set so far */
    BITVEC_ITER(occ, v) puts(vertices[v]);
    exit(0);
}


void find_occ(const struct graph *g)
{
    verbose = false;

    occ = bitvec_make(g->size);
    ALLOCA_BITVEC(sub, g->size);


    /* sub should be the current subgraph and occ the current oct set.
       We hardcode some preprocessing below: */

    for (size_t i = 0; i < g->size; ++i)
    {
        // Add i to the subgraph we're looking at
	    bitvec_set(sub, i);
	    struct graph *g2 = graph_subgraph(g, sub);

        // // If this is already an OCT set, continue on
	    if (occ_is_occ(g2, occ))
        {
	        graph_free(g2);
	        continue;
	    }

        // Otherwise add i to the OCT set and compress
	    bitvec_set(occ, i);
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
	        free(occ);
	        occ = occ_new;
	        if (!occ_is_occ(g2, occ))
            {
		        fprintf(stderr, "Internal error!\n");
		        abort();
	        }
	    }
	    graph_free(g2);
    }
}


int main(int argc, char *argv[]) {
    /* Set up the SIGTERM handler */
    struct sigaction action;
    memset(&action, 0, sizeof(struct sigaction));
    action.sa_handler = term;
    sigaction(SIGTERM, &action, NULL);

    int c;
    while ((c = getopt(argc, argv, "bgvsh")) != -1)
    {
	    switch (c)
        {
	        case 'b': enum2col   = true; break;
	        case 'g': use_gray   = true; break;
	        case 'v': verbose    = true; break;
	        case 'h': usage(stdout); exit(0); break;
	        default:  usage(stderr); exit(1); break;
	    }
    }

    struct graph *g = graph_read(stdin, &vertices);
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
