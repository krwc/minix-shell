#include "message.h"
#include "prompt.h"

void message_error(const char* program, const char* message)
{
    write(STDERR, program, strlen(program));
    write(STDERR, ": ", 2);
    write(STDERR, message, strlen(message));
    print_prompt();
}

void message(int output, const char* message)
{
    write(STDERR, message, strlen(message));
    print_prompt();
}
