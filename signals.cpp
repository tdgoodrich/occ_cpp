#include "signals.hpp"

sigset_t signalset;
volatile sig_atomic_t received_timeout;

// Initialize signal handler
void init_signal_handler() {

    // No timeout received
    received_timeout = 0;

    // Set term to be SIGTERM handler
    struct sigaction action;
    memset(&action, 0, sizeof(struct sigaction));
    sigfillset(&signalset);
    action.sa_handler = term;
    action.sa_mask = signalset;
    sigaction(SIGTERM, &action, NULL);

}


/* How to respond to a SIGTERM */
void term(int signum)
{

    // Only handle sigterm
    if (signum != SIGTERM) return;

    // Mark that a (SIGTERM) timeout has been received
    received_timeout = 1;

}
