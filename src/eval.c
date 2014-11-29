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
#include "main.h"
#include "signal.h"

bool eval_command_has_line(Command* cmd)
{
    if (cmd && cmd->argv[0] == NULL)
        return false;
    return true;
}

static int eval_open_file(const char* path, int flags, int oflags)
{
    int fd = open(path, flags, oflags);
    int error;

    if (fd == -1)
    {
        error = errno;
        
        switch (error)
        {
        case EACCES:
            message_error(path, EVAL_ERROR_EACCES);
            break;
        case ENOENT:
            message_error(path, EVAL_ERROR_ENOENT);
            break;
        default:
            message_error(path, EVAL_ERROR_UHNDLD);
            break;
        }
    }
    return fd;
}

// Function redirects STDOUT to the file if it is possible,
// otherwise does nothing and returns false.
static bool eval_redir_to_file(Command* cmd)
{
    int flags = O_CREAT | O_WRONLY;
    if (cmd->output.type == STREAM_FILE)
        flags |= O_TRUNC;
    else if (cmd->output.type == STREAM_FILE_APPEND)
        flags |= O_APPEND;

    int ret = eval_open_file(cmd->output.file, flags,
                             S_IWUSR | S_IRUSR | S_IRGRP | S_IRGRP | S_IROTH);
    if (ret != -1)
    {
        dup2(ret, STDOUT);
        close(ret);
        return true;
    }
    return false;
}

// Function redirects STDIN from file if it is possible,
// otherwise does nothing and returns false.
static bool eval_redir_from_file(Command* cmd)
{
    int ret = eval_open_file(cmd->input.file, O_RDONLY, 0);
    if (ret != -1)
    {
        dup2(ret, STDIN);
        close(ret);
        return true;
    }
    return false;
}

static void eval_handle_exec_error_if_any(const char* appname, int ret)
{
    int error = errno;
    switch (error)
    {
    case ENOENT:
        message_error(appname, EVAL_ERROR_ENOENT);
        break;
    case EACCES:
        message_error(appname, EVAL_ERROR_EACCES);
        break;
    default:
        message_error(appname, EVAL_ERROR_EXEC);
        break;
    }
    exit(ret);
}

int eval(Command* cmd, int fd[2], bool background)
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
    signal_block(SIGCHLD);

    pid_t child = fork();
    if (child == 0)
    {
        if (background)
            setsid();

        signal_reset_handler(SIGINT);
        signal_reset_handler(SIGCHLD);

        if (cmd->input.type == STREAM_PIPE)
        {
            if (fd[STDIN] != STDIN)
            {
                dup2(fd[STDIN], STDIN);
                close(fd[STDIN]);
            }
        }
        else if (cmd->input.type == STREAM_FILE)
            if (!eval_redir_from_file(cmd))
                exit(EXIT_FAILURE);
        
        if (cmd->output.type == STREAM_PIPE)
        {
            if (fd[STDOUT] != STDOUT)
            {
                dup2(fd[STDOUT], STDOUT);
                close(fd[STDOUT]);
            }
        }
        else if (cmd->output.type == STREAM_FILE || cmd->output.type == STREAM_FILE_APPEND)
            if (!eval_redir_to_file(cmd))
                exit(EXIT_FAILURE);

        eval_handle_exec_error_if_any(cmd->argv[0], execvp(cmd->argv[0], cmd->argv));
    } else {
        if (background)
            children_bg[num_children_bg++] = child;
        else
            children_fg[num_children_fg++] = child;
        signal_block(SIGCHLD);
    }
    return 1;
}

