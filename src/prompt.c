#include "prompt.h"
#include "config.h"

void print_prompt()
{
    struct stat input_stat;
    fstat(STDIN, &input_stat);
    
    // Write prompt if stdin is a character-device.
    if (S_ISCHR(input_stat.st_mode))
        write(STDOUT, "> ", 3);
}

