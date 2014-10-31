#include "prompt.h"
#include "config.h"
#include "main.h"
#include <stdio.h>
#include <stdlib.h>

void prompt_prepare()
{
    // If shell_workdir begins with shell_home, print '~'
    if (!strncmp(shell_home, shell_workdir, strlen(shell_home)))
    {
        snprintf(prompt_buffer, MAX_PROMPT_LENGTH - 1, "%s~%s %s$%s ", COLOR_BLUE,
                 shell_workdir + strlen(shell_home), COLOR_GREEN, COLOR_NONE);
    }
    else
    {
        snprintf(prompt_buffer, MAX_PROMPT_LENGTH - 1, "%s%s %s$%s ", COLOR_BLUE,
                 shell_workdir, COLOR_GREEN, COLOR_NONE);
    }
}

void print_prompt()
{
    struct stat input_stat;
    fstat(STDIN, &input_stat);
    
    // Write prompt if stdin is a character-device.
    if (S_ISCHR(input_stat.st_mode))
    {
        prompt_prepare();
        write(STDOUT, prompt_buffer, strlen(prompt_buffer));
    }
}

