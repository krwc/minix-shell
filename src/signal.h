#ifndef SIGNAL_H
#define SIGNAL_H
#include <signal.h>
#include <assert.h>
#include <stdbool.h>
// Initializes signal handling
void signal_init();
// Returns default signal set
sigset_t signal_get_default_sigset();
// Blocks given signal
void signal_block(int signum);
// Unblocks given signal
void signal_unblock(int signum);
// Sets signal handler
void signal_set_handler(int signum, void (*handler)(int));
// Restores default signal handler
void signal_reset_handler(int signum);

#endif
