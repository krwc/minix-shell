#include <unistd.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include "main.h"
#include "message.h"
#include "prompt.h"
#include "input.h"
#include "config.h"
#include "eval.h"
#include "signal.h"
#include "parser-core/parser.h"
// HOME directory
char shell_home[MAX_DIR_LENGTH];
// Shell current working-directory
char shell_workdir[MAX_DIR_LENGTH];

// Children in the foreground
pid_t children_fg[MAX_CHILDREN];
volatile int num_children_fg;
// Children in the background
pid_t children_bg[MAX_CHILDREN];
volatile int num_children_bg;
// Children in the background that terminated
pid_t children_bg_terminated[MAX_CHILDREN];
int children_bg_terminated_status[MAX_CHILDREN];
int num_children_bg_terminated;

static bool children_in_fg(pid_t pid)
{
    for (int i = 0; i < MAX_CHILDREN; i++)
        if (children_fg[i] == pid)
            return true;
    return false;
}

static void sigint_handler(int signum)
{
    // We must ignore ignore commands till the end of the line.
    parser_ignore_line();
}

static void sigchld_handler(int signum)
{
    // Some children was just killed
    int status;
    pid_t pid;
    while ((pid = waitpid(-1, &status, WNOHANG)) > 0)
    {
        if (children_in_fg(pid))
            --num_children_fg;
        else
        {
            if (prompt_should_print())
            {
                children_bg_terminated[num_children_bg_terminated] = pid;
                children_bg_terminated_status[num_children_bg_terminated++] = status;
            }
        }
    }
}

void wait_for_foreground()
{
    const sigset_t mask = signal_get_default_sigset();
    assert(num_children_fg >= 0 && "num_children_fg should not be negative");
    while (num_children_fg)
        sigsuspend(&mask);
}


void init_shell_globals()
{
    strcpy(shell_home, getenv("HOME"));
    getcwd(shell_workdir, MAX_DIR_LENGTH - 1);
    prompt_init();
    signal_init();
    signal_set_handler(SIGINT, sigint_handler);
    signal_set_handler(SIGCHLD, sigchld_handler);
}

void kill_remaining_children()
{
    for (int i = 0; i < num_children_fg; i++)
        kill(children_fg[i], SIGKILL);
    for (int i = 0; i < num_children_bg; i++)
        kill(children_bg[i], SIGKILL);
}

int main()
{
    init_shell_globals();

    print_prompt();
    const char* line = NULL;

    int p[2];
    #define PIPE_OUT 0
    #define PIPE_IN  1
    
    while ((line = input_get_line()) != NULL)
    {
        parser_parse_line(line);
        Pipeline pipeline;
        Command* command;
        bool background = parser_in_background(); 
        
        while ((pipeline = parser_get_pipeline()))
        {
            int command_id = 0;
            int fd[2];
            fd[STDIN]  = STDIN;
            fd[STDOUT] = STDOUT;
            signal_block(SIGCHLD);
            signal_block(SIGINT);
            while ((command = parser_get_command()))
            {
                if (command->input.type  == STREAM_PIPE)
                    fd[STDIN] = p[PIPE_OUT];
                if (command->output.type == STREAM_PIPE)
                {
                    pipe(p);
                    fd[STDOUT] = p[PIPE_IN];
                }
                eval(command, fd, background);
                if (fd[STDOUT] != STDOUT)
                    close(fd[STDOUT]);
                if (fd[STDIN] != STDIN)
                    close(fd[STDIN]);
                parser_next_command();
                ++command_id;
            }
            if (!background)
                wait_for_foreground();
            signal_unblock(SIGCHLD);
            signal_unblock(SIGINT);
            parser_next_pipeline();
        }
        print_prompt();
    }
    kill_remaining_children();

    return 0;
}
