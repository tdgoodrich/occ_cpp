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

#ifndef GRAPH_H
#define GRAPH_H

#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>

struct bitvec;

/* uint16_t would do, but seems to be slower in benchmarks.  */
typedef size_t vertex;

#define NULL_NEIGHBORS ((struct vertex *) (size_t) 1)

struct graph {
    size_t capacity;
    size_t size;
    struct vertex {
        /* This used to be vertex, not int */
	    size_t capacity;
	    size_t deg;
	    size_t neighbors[];
    } *vertices[];
};

#define GRAPH_NEIGHBORS_ITER(g, v, w)					\
    for (vertex *__pw = g->vertices[v]->neighbors, *__pw_end = __pw + g->vertices[v]->deg; \
	 __pw != __pw_end && (w = *__pw, 1); __pw++)

#define GRAPH_ITER_EDGES(g, v, w)				\
    for (v = 0; v < g->size; v++)				\
	if (graph_vertex_exists(g, v))				\
	    for (vertex *__pw = g->vertices[v]->neighbors,	\
		        *__pw_end = __pw + g->vertices[v]->deg;	\
		 __pw != __pw_end && (w = *__pw, 1); __pw++)	\
		if (v < w)


struct graph *graph_make(size_t n);
struct graph *graph_copy(const struct graph *g);
struct graph *graph_grow(struct graph *g, size_t size);
struct graph *graph_subgraph(const struct graph *g, const struct bitvec *s);
void graph_free(struct graph *g);

static inline size_t graph_size(const struct graph *g) { return g->size; }
size_t graph_num_vertices(const struct graph * g);
size_t graph_num_edges(const struct graph * g);
static inline bool graph_vertex_exists(const struct graph *g, vertex v) {
    assert(v < g->size);
    return ((size_t) g->vertices[v] & 1) == 0;
}
bool graph_is_bipartite(const struct graph *g);
bool graph_two_coloring(const struct graph *g, struct bitvec *colors);

void graph_connect(struct graph *g, vertex v, vertex w);
void graph_disconnect(struct graph *g, vertex v, vertex w);
void graph_vertex_disable(struct graph *g, vertex v);
void graph_vertex_enable(struct graph *g, vertex v);

void graph_output(const struct graph *g, FILE* stream, const char **vertices);
static inline void graph_print(const struct graph *g, const char **vertices) {
    graph_output(g, stdout, vertices);
}
static inline void graph_dump(const struct graph *g, const char **vertices) {
    graph_output(g, stderr, vertices);
}
struct graph *graph_read(FILE* stream, const char ***vertices_out);

#endif // GRAPH_H
