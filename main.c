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
	    "  -s  Print only statistics\n"
	    "  -h  Display this list of options\n"
	);
}

bool enum2col   = false;
bool use_gray   = false;
bool verbose    = false;
bool stats_only = false;
unsigned long long augmentations = 0;

struct bitvec *find_hardcoded_occ(const struct graph *g)
{
    verbose = true;
    struct bitvec *occ = NULL;

    occ = bitvec_make(g->size);
    ALLOCA_BITVEC(sub, g->size);

    /* sub should be the current subgraph and occ the current oct set.
       We hardcode some preprocessing below: */

    size_t partite_a[213] =  { 5, 6, 12, 13, 17, 22, 25, 31, 32, 33, 34, 35, 36, 39, 41, 42, 43, 44, 46, 47, 49, 51, 61, 62, 63, 70, 72, 73, 74, 75, 76, 77, 78, 79, 81, 83, 85, 88, 89, 90, 91, 92, 105, 115, 117, 124, 125, 126, 127, 128, 129, 130, 131, 132, 133, 134, 135, 136, 137, 138, 139, 140, 141, 142, 143, 144, 145, 146, 147, 149, 150, 151, 152, 153, 154, 155, 156, 157, 158, 159, 160, 161, 162, 163, 164, 165, 166, 167, 168, 169, 170, 171, 172, 173, 174, 175, 176, 177, 178, 179, 180, 181, 182, 183, 184, 185, 186, 187, 188, 189, 190, 191, 192, 193, 194, 195, 196, 197, 198, 199, 200, 201, 202, 203, 204, 205, 206, 207, 208, 209, 210, 211, 212, 213, 214, 215, 216, 217, 218, 219, 220, 221, 222, 223, 224, 225, 226, 227, 228, 229, 230, 234, 235, 236, 237, 238, 239, 240, 241, 242, 243, 244, 245, 246, 247, 248, 249, 250, 251, 252, 253, 254, 255, 256, 257, 258, 259, 260, 261, 262, 263, 264, 265, 266, 267, 268, 269, 270, 271, 272, 273, 274, 275, 276, 277, 278, 279, 280, 281, 282, 283, 284, 285, 286, 287, 288, 289, 290, 291, 292, 293, 294, 295};

    size_t partite_b[36] = { 0, 3, 4, 7, 11, 21, 23, 26, 28, 29, 30, 50, 55, 57, 60, 71, 80, 82, 96, 97, 99, 101, 102, 106, 108, 110, 111, 113, 116, 119, 120, 123, 148, 231, 232, 233};

    size_t remaining[47] = { 1, 2, 8, 9, 10, 14, 15, 16, 18, 19, 20, 24, 27, 37, 38, 40, 45, 48, 52, 53, 54, 56, 58, 59, 64, 65, 66, 67, 68, 69, 84, 86, 87, 93, 94, 95, 98, 100, 103, 104, 107, 109, 112, 114, 118, 121, 122};

    for (size_t i = 0; i < 213; ++i)
    {
        bitvec_set(sub, partite_a[i]);
    }

    for (size_t i = 0; i < 36; ++i)
    {
        bitvec_set(sub, partite_b[i]);
    }

    for (size_t i = 0; i < 47; ++i)
    {
        // Add i to the subgraph we're looking at
	    bitvec_set(sub, remaining[i]);
	    struct graph *g2 = graph_subgraph(g, sub);

        // If this is already an OCT set, continue on
	    if (occ_is_occ(g2, occ))
        {
	        graph_free(g2);
	        continue;
	    }

        // Otherwise add i to the OCT set and compress
	    bitvec_set(occ, remaining[i]);
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
    return occ;
}

struct bitvec *find_occ(const struct graph *g)
{
    verbose = true;
    struct bitvec *occ = NULL;

    occ = bitvec_make(g->size);
    ALLOCA_BITVEC(sub, g->size);


    /* sub should be the current subgraph and occ the current oct set.
       We hardcode some preprocessing below: */

    for (size_t i = 0; i < g->size; ++i)
    {
        // Add i to the subgraph we're looking at
	    bitvec_set(sub, i);
	    struct graph *g2 = graph_subgraph(g, sub);

        // If this is already an OCT set, continue on
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
    return occ;
}

int main(int argc, char *argv[]) {
    int c;
    while ((c = getopt(argc, argv, "bgvsh")) != -1)
    {
	    switch (c)
        {
	        case 'b': enum2col   = true; break;
	        case 'g': use_gray   = true; break;
	        case 'v': verbose    = true; break;
	        case 's': stats_only = true; break;
	        case 'h': usage(stdout); exit(0); break;
	        default:  usage(stderr); exit(1); break;
	    }
    }

    const char **vertices;
    struct graph *g = graph_read(stdin, &vertices);
    size_t occ_size;

    struct bitvec *occ = find_occ(g);

    occ_size = bitvec_count(occ);
    if (!stats_only)
    {
	    BITVEC_ITER(occ, v) puts(vertices[v]);
    }

    if (stats_only)
    {
	       printf("%5lu %6lu %5lu %10.2f %16llu\n",
	       (unsigned long) g->size, (unsigned long) graph_num_edges(g),
	       (unsigned long) occ_size, user_time(), augmentations);
    }

    return 0;
}
