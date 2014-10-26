#ifndef _INPUT_H_
#define _INPUT_H_
#include "config.h"
#include <unistd.h>
#include <sys/stat.h>
#include <stdbool.h>
#include "message.h"

typedef struct Input
{
    // Amount of bytes read
    size_t size;
} Input;

const char* input_get_line();

#endif
