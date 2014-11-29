#include "prompt.h"
#include "config.h"
#include "main.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
static bool has_char_device = false;

void prompt_init()
{
    struct stat input_stat;
    fstat(STDIN, &input_stat);

    has_char_device = S_ISCHR(input_stat.st_mode);
}

const char* prompt_get_username()
{
    static char name[256];
    getlogin_r(name, 255);
    return name;
}

const char* prompt_get_hostname()
{
    static char hostname[256];
    gethostname(hostname, 255);
    return hostname;
}

const char* prompt_get_workdir()
{
    static char workdir[256];

    // If workdir starts with $HOME, then we append ~.
    if (!strncmp(shell_home, shell_workdir, strlen(shell_home)))
        snprintf(workdir, 255, "~%s", shell_workdir + strlen(shell_home));
    else
        snprintf(workdir, 255, "%s", shell_workdir);
    return workdir;
}

char prompt_get_usermode()
{
    return getuid() == 0 ? '#' : '$';
}

void prompt_append_char(char character, size_t* offset)
{
    if (*offset < MAX_PROMPT_LENGTH)
    {
        prompt_buffer[(*offset)++] = character;
        prompt_buffer[*offset] = '\0';
    }
}

void prompt_append_string(const char* string, size_t* offset)
{
    for (size_t i = 0; i < strlen(string); i++)
        prompt_append_char(string[i], offset);
}

void prompt_prepare(const char* prompt_declaration)
{
    // Prompt buffer iterator
    size_t i = 0;

    for (const char* ch = prompt_declaration; *ch; ch++)
    {
        if (*ch == '\\')
        {
            char escaped_char = *(ch + 1);
          
            if (escaped_char == PROMPT_MOD_HOST)
                prompt_append_string(prompt_get_hostname(), &i);
            else if (escaped_char == PROMPT_MOD_USER)
                prompt_append_string(prompt_get_username(), &i);
            else if (escaped_char == PROMPT_MOD_MODE)
                prompt_append_char(prompt_get_usermode(), &i);
            else if (escaped_char == PROMPT_MOD_WORKDIR)
                prompt_append_string(prompt_get_workdir(), &i); 
            else
            {
                prompt_append_char(*ch, &i);
                continue;
            }
            ch++;
        }
        else
            prompt_append_char(*ch, &i);
    }
}

bool prompt_should_print()
{
    return has_char_device;
}

void print_prompt()
{
    // Write prompt if stdin is a character-device.
    if (prompt_should_print())
    {
        signal_block(SIGCHLD);
        for (int i = 0; i < num_children_bg_terminated; i++)
        {
            const int status = children_bg_terminated_status[i];
            const pid_t pid  = children_bg_terminated[i];
            const char* reason = WIFEXITED(status) ? "exited with status" : "killed by signal";
            const int statusno = WIFEXITED(status) ? status : WTERMSIG(status);
            fprintf(stdout, "Background process %d terminated. (%s %d)\n", pid, reason, status);
        }
        num_children_bg_terminated = 0;
        signal_unblock(SIGCHLD);

        prompt_prepare(PROMPT_DEFAULT);
        fprintf(stdout, "%s", prompt_buffer);
        fflush(stdout);
    }
}

