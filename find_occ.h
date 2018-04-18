#ifndef FIND_OCC_H
#define FIND_OCC_H

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <sys/times.h>
#include <signal.h>
#include "bitvec.h"
#include "graph.h"
#include "occ.h"


/* Always use enum2col */
extern bool enum2col;
extern bool use_gray;
extern struct bitvec *occ;
extern const char **vertices;
extern unsigned long long augmentations;
extern struct graph *g;

size_t last_node_finished;
sigset_t signalset;

double user_time(void);
void block();
void unblock();
void term(int signum);
void find_occ(const struct graph *g);

#endif
