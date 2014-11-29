#include "signal.h"
#include <stdlib.h>
#include <signal.h>
// Supported signals
static const int signals[] = { SIGCHLD, SIGINT };
static const int num_signals = sizeof(signals) / sizeof(int);
// Signal actions
static struct sigaction default_act[num_signals];
static struct sigaction current_act[num_signals];
// Initial signal mask
static sigset_t default_sigset;

sigset_t signal_get_default_sigset()
{
    return default_sigset;
}

static int signal_get_id(int signum)
{
    for (int i = 0; i < num_signals; i++)
        if (signals[i] == signum)
            return i;
    assert(false && "signum has invalid value");
}

void signal_init()
{
    sigprocmask(SIG_BLOCK, NULL, &default_sigset);
}

void signal_block(int signum)
{
    sigset_t sigset;
    sigemptyset(&sigset);
    sigaddset(&sigset, signum);
    sigprocmask(SIG_BLOCK, &sigset, NULL);
}

void signal_unblock(int signum)
{
    sigset_t sigset;
    sigemptyset(&sigset);
    sigaddset(&sigset, signum);
    sigprocmask(SIG_UNBLOCK, &sigset, NULL);
}

void signal_set_handler(int signum, void (*handler)(int))
{
    const int signal_id = signal_get_id(signum);
    // Set signal handler
    current_act[signal_id].sa_handler = handler;
    // Block all signals when handler is being executed
    sigfillset(&current_act[signal_id].sa_mask);
    // Apply settings, and retrieve default handler
    sigaction(signum, &current_act[signal_id], &default_act[signal_id]);
}

void signal_reset_handler(int signum)
{
    const int signal_id = signal_get_id(signum);
    sigaction(signum, NULL, &default_act[signal_id]);
}

