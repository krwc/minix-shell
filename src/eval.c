#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#include <errno.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "eval.h"
#include "builtins.h"
#include "message.h"

bool eval_command_has_line(Command* cmd)
{
    if (cmd && cmd->argv[0] == NULL)
        return false;
    return true;
}

static void eval_begin_setup_io(Command* cmd, int fd[2])
{
    if (cmd->input.type == STREAM_PIPE_OUT)
    {
        printf("%s has input: %d\n", cmd->argv[0], fd[STDIN]);
        close(STDIN);
        dup2(fd[STDIN], STDIN);
    }
    else if (cmd->input.type == STREAM_FILE)
    {
        close(STDIN);
        dup2(open(cmd->input.file, O_RDONLY), STDIN);
    }

    if (cmd->output.type == STREAM_PIPE_IN)
    {
        printf("%s has output: %d\n", cmd->argv[0], fd[STDOUT]);

        close(STDOUT);
        dup2(fd[STDOUT], STDOUT);
    }
    else
    {
        if (cmd->redirs[0])
        {
            int flags = O_CREAT | O_WRONLY;
            if (cmd->output.type == STREAM_FILE)
                flags |= O_TRUNC;
            if (cmd->output.type == STREAM_FILE_APPEND)
                flags |= O_APPEND;

            dup2(open(cmd->output.file, flags, S_IWUSR | S_IRUSR | S_IRGRP | S_IRGRP | S_IROTH), STDOUT);
        }
    }
}

static void eval_end_setup_io(Command* cmd, int fd[2])
{
    if (cmd->output.type == STREAM_PIPE_IN)
        close(fd[STDOUT]);
    if (cmd->input.type == STREAM_PIPE_OUT)
        close(fd[STDIN]);
}

int eval(Command* cmd)
{
    // When parsed line is a comment, we simply can ignore it now.
    if (!eval_command_has_line(cmd))
        return 0;
    
    // If it is builtin function, execute it and return;
    for (Builtin* builtin = builtin_list; builtin->name != NULL; builtin++)
    {
        if (!strcmp(builtin->name, cmd->argv[0]))
        {
            if (builtin->callback(cmd->argc, cmd->argv) != 0)
                fprintf(stderr, "Builtin %s error.\n", builtin->name);
            return 0;
        }
    }
    int child_status;
    // Shall we be using previous pipe output?
    static int prev_fd[2];
    static int curr_fd[2];
    int fd[2];
    
    // We're sending our output to the pipe input
    if (cmd->output.type == STREAM_PIPE_IN)
    {
        prev_fd[0] = curr_fd[0];
        prev_fd[1] = curr_fd[1];
        pipe(curr_fd);
        fd[STDOUT] = curr_fd[STDOUT];
        fd[STDIN]  = curr_fd[STDIN];
    }
    // Some pipe manipulation...
    if (cmd->input.type == STREAM_PIPE_OUT)
    {
        if (cmd->output.type == STREAM_PIPE_IN)
            fd[STDIN]  = prev_fd[STDIN];
        else
            fd[STDIN]  = curr_fd[STDIN];
    }

    pid_t child = fork();
    if (child == 0)
    {
        // Setup input / output properly
        if (cmd->input.type == STREAM_PIPE_OUT)
        {
//            printf("%s has input: %d\n", cmd->argv[0], fd[STDIN]);
            close(STDIN);
            dup2(fd[STDIN], STDIN);
        }
        else if (cmd->input.type == STREAM_FILE)
        {
            close(STDIN);
            dup2(open(cmd->input.file, O_RDONLY), STDIN);
        }

        if (cmd->output.type == STREAM_PIPE_IN)
        {
//            printf("%s has output: %d\n", cmd->argv[0], fd[STDOUT]);

            close(STDOUT);
            dup2(fd[STDOUT], STDOUT);
        }
        else
        {
            if (cmd->redirs[0])
            {
                int flags = O_CREAT | O_WRONLY;
                if (cmd->output.type == STREAM_FILE)
                    flags |= O_TRUNC;
                if (cmd->output.type == STREAM_FILE_APPEND)
                    flags |= O_APPEND;

                dup2(open(cmd->output.file, flags,
                          S_IWUSR | S_IRUSR | S_IRGRP | S_IRGRP | S_IROTH),
                     STDOUT);
            }
        }

        int ret = execvp(cmd->argv[0], cmd->argv);
        int error = errno;

        // An error occured
        if (ret == -1)
        {
            switch (error)
            {
            case ENOENT:
                message_error(cmd->argv[0], "no such file or directory\n"); break;
            case EACCES:
                message_error(cmd->argv[0], "permission denied\n"); break;
            default:
                message_error(cmd->argv[0], "exec error\n"); break;
            }
            exit(ret);
        }
    } else {
       eval_end_setup_io(cmd, fd);
    }
    return 1;
}

