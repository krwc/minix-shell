#include <unistd.h>
#include <sys/stat.h>
#include <string.h>
#include <stdbool.h>
#include "message.h"
#include "prompt.h"
#include "input.h"
#include "config.h"
#include "eval.h"
#include "parser-core/parser.h"

int main()
{
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
