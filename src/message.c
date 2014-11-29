#include "message.h"
#include "prompt.h"
#include <stdio.h>

void message_error(const char* program, const char* message)
{
    fprintf(stderr, "%s: %s", program, message);
    fflush(stderr);
}

void message(FILE* output, const char* message)
{
    fprintf(output, "%s", message);
    fflush(output);
    print_prompt();
}
