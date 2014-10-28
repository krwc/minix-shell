#include "parser.h"
static Parser g_parser;

void parser_parse_line(const char* input)
{
    g_parser.parsed_line = parseline((char*)input);
    g_parser.pipeline = 0;
    g_parser.command = 0;

    Pipeline pipeline;
    Command* command;
    while ((pipeline = parser_get_pipeline()))
    {
        while ((command = parser_get_command()))
        {
            command->argc = 0;
            for (int i = 0; command->argv[i] != NULL; i++)
                ++command->argc; 
            parser_next_command();
        }
        parser_next_pipeline();
    }
    g_parser.pipeline = 0;
    g_parser.command = 0;
}

Pipeline parser_get_pipeline()
{
    return g_parser.parsed_line->pipelines[g_parser.pipeline];
}

Command* parser_get_command()
{
    return parser_get_pipeline()[g_parser.command];
}

void parser_next_pipeline()
{
    if (parser_get_pipeline())
    {
        g_parser.command = 0;
        g_parser.pipeline++;
    }
}

void parser_next_command()
{
    if (parser_get_command())
        g_parser.command++;
}

