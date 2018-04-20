#ifndef FIND_OCC_H
#define FIND_OCC_H

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <sys/times.h>
#include <vector>
#include <chrono>
#include <random>

extern "C" {
    #include "bitvec.h"
    #include "graph.h"
    #include "occ.h"
}

#include "signals.hpp"
#include "heuristics/Ensemble.hpp"
#include "heuristics/Graph.hpp"


/* Always use enum2col */
extern bool enum2col;
extern bool use_gray;
extern struct bitvec *occ;
extern const char **vertices;
extern unsigned long long augmentations;
extern struct graph *g;

double user_time(void);
void find_occ(const struct graph *g, int preprocessing, int seed, long htime);

#endif
