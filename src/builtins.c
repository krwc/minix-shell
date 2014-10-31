#include "builtins.h"
#include "message.h"
#include "main.h"
#include <signal.h>
#include <dirent.h>

Builtin builtin_list[] = { 
    { "exit", &builtin_exit },
    { "lecho", &builtin_echo },
    { "lcd", &builtin_cd },
    // Alias
    { "cd", &builtin_cd }, 
    { "lkill", &builtin_kill },
    { "lls", &builtin_ls },
    { NULL, NULL } 
};

int builtin_exit(int argc, char** argv)
{
    exit(0);
}

int builtin_echo(int argc, char** argv)
{
    static char data[MAX_LINE_LENGTH];
    sprintf(data, "%s\n", argv[1]);
    write(STDOUT, data, strlen(data));

    return 0;
}

int builtin_cd(int argc, char** argv)
{
    if (argc > 2)
        return -1;
    const char* path = argv[1] ? argv[1] : getenv("HOME");
    
    int ret = chdir(path);

    if (ret != -1)
    {
        memcpy(shell_workdir, path, strlen(path));
        getcwd(shell_workdir, MAX_DIR_LENGTH);
    }

    return ret;
}

int builtin_kill(int argc, char** argv)
{
    if (argc < 2)
        return -1;
    int signal_id = SIGTERM;
    int pid = 0;

    // Signal specified
    if (argv[1][0] == '-') 
    {
        signal_id = atoi(argv[1] + 1);
        pid = atoi(argv[2]);
    }
    else
        pid = atoi(argv[1]);
    
    return kill(pid, signal_id);
}

int builtin_ls(int argc, char** argv)
{
    DIR* dir = opendir(".");

    if (!dir)
        return -1;
    
    struct dirent* entry;

    do {
        entry = readdir(dir);
        
        if (entry)
        {
            if (entry->d_name[0] != '.')
               fprintf(stdout, "%s\n", entry->d_name);
        }
    } while (entry != NULL);
    closedir(dir);

    return 0;
}

