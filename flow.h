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

#ifndef FLOW_H
#define FLOW_H

#include <stdbool.h>

#include "graph.h"

struct flow;

struct flow* flow_make(const struct graph *g);
void flow_clear(struct flow *flow);
void flow_free(struct flow *flow);

size_t flow_flow(const struct flow *flow);
bool flow_vertex_flow(const struct flow *flow, vertex v);
bool flow_is_source(const struct flow *flow, vertex v);
bool flow_is_target(const struct flow *flow, vertex v);

bool flow_augment(struct flow *flow, const struct bitvec *sources,
		  const struct bitvec *targets);
bool flow_augment_pair(struct flow *flow, vertex source, vertex target);
vertex flow_drain_source(struct flow *flow, vertex source);
vertex flow_drain_target(struct flow *flow, vertex target);
struct bitvec *flow_vertex_cut(const struct flow *flow,
			       const struct bitvec *sources);
void flow_dump(const struct flow *flow);

#endif
