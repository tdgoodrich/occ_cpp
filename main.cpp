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


#if defined (__SVR4) && defined (__sun)
int getopt(int argc, const char *argv[], const char *optstring);
#else
# include <getopt.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <sys/times.h>
#include <signal.h>


// Include local C header files
extern "C" {
    #include "bitvec.h"
    #include "graph.h"
    #include "occ.h"
    #include "find_occ.h"
}


// Include CPP header files
#include "heuristics/Ensemble.hpp"


/* The actual parameters */
bool verbose    = false;
char *graph_filename;
int preprocessing_level = 0;


void usage(FILE *stream) {
    fprintf(stream,
	    "occ: Calculate minimum odd cycle cover\n"
	    "  -v  Print progress to stderr\n"
        "  -f  Compute OCT on this graph file"
	    "  -h  Display this list of options\n"
        "  -p  Preprocessing level {0: None, 1: Bipartite, 2: Bipartite + Density Sort}\n"
	);
}


int main(int argc, char *argv[]) {

    EnsembleSolver solver;

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
