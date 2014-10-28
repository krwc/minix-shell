#ifndef _BUILTINS_H_
#define _BUILTINS_H_
#include "config.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

typedef struct Builtin
{
    const char* name;
    int (*callback)(int argc, char** argv);
} Builtin;
extern Builtin builtin_list[];

int builtin_exit(int, char**);
int builtin_echo(int, char**);
int builtin_cd(int, char**);
int builtin_kill(int, char**);
int builtin_ls(int, char**);


#endif
