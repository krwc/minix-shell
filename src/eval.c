#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#include <errno.h>
#include <sys/wait.h>
#include "eval.h"

bool eval_command_has_line(Command* cmd)
{
    if (cmd && cmd->argv[0] == NULL)
        return false;
    return true;
}


void eval(Command* cmd)
{
    // When parsed line is a comment, we simply can ignore it now.
    if (!eval_command_has_line(cmd))
        return;

    int child_status;
    pid_t child = fork();

    if (child == 0)
    {
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
    } else
        wait(&child_status);
}

