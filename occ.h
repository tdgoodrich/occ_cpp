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

#ifndef OCC_H
#define OCC_H

#include <stdbool.h>

#include "graph.h"

struct bitvec;
struct flow;

struct occ_problem
{
    const struct graph *g;	// input graph
    struct graph *h;		// G' as described by Reed et al.
    const struct bitvec *occ;	// known odd cycle cover for g
    vertex *occ_vertices;	// array of the k vertices in occ
    vertex *clones;		// clones[v] contains the clone of [v] or 0
    struct bitvec *sources, *targets; // for the flow
    struct flow *flow;
    size_t num_sources;
    bool use_graycode;
    bool last_not_in_occ;
    size_t occ_size, first_clone;
};

bool occ_is_occ(const struct graph *g, const struct bitvec *occ);
struct bitvec *occ_shrink(const struct graph *g, const struct bitvec *occ,
    bool enum2col, bool use_graycode, bool last_not_in_occ);
struct bitvec *occ_heuristic(const struct graph *g);
struct bitvec *occ_shrink_gray(struct occ_problem *problem);
struct bitvec *occ_shrink_enum2col(struct occ_problem *problem);

#endif // OCC_H
