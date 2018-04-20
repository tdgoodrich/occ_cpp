#ifndef SIGNALS_H
#define SIGNALS_H

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <vector>
#include <cstring>

extern sigset_t signalset;
extern volatile sig_atomic_t received_timeout;

void init_signal_handler();
void term(int signum);

#endif
