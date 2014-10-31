#include <unistd.h>
#include <sys/stat.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include "main.h"
#include "message.h"
#include "prompt.h"
#include "input.h"
#include "config.h"
#include "eval.h"
#include "parser-core/parser.h"
// HOME directory
char shell_home[MAX_DIR_LENGTH];
// Shell current working-directory
char shell_workdir[MAX_DIR_LENGTH];

void init_shell_globals()
{
    strcpy(shell_home, getenv("HOME"));
    getcwd(shell_workdir, MAX_DIR_LENGTH - 1);
}

int main()
{
    init_shell_globals();

    print_prompt();
    const char* line = NULL;
    
    while ((line = input_get_line()) != NULL)
    {
        parser_parse_line(line);
        Pipeline pipeline;
        Command* command;
        
        while ((pipeline = parser_get_pipeline()))
        {
            while ((command = parser_get_command()))
            {
                eval(command);
                parser_next_command();
            }
            parser_next_pipeline();
        }
        print_prompt();
    }
    return 0;
}
