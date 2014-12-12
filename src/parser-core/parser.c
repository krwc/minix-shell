#include "parser.h"
#include "../message.h"
#include <string.h>
static Parser g_parser = {
  .parsed_line = NULL,
  .pipeline = 0,
  .command = 0,
  .background = false
};

void parser_parse_line(const char* input)
{
    g_parser.parsed_line = parseline((char*)input);
    g_parser.pipeline = 0;
    g_parser.command = 0;
    if (g_parser.parsed_line == NULL)
    {
        fprintf(stderr, "%s", MSG_SYNTAX_ERROR);
        fflush(stderr);
        return;
    }
    g_parser.background = g_parser.parsed_line->flags == LINBACKGROUND;

    Pipeline pipeline;
    Command* command;
    while ((pipeline = parser_get_pipeline()))
    {
        int command_id = 0;
        while ((command = parser_get_command()))
        {
            if (command->argv[0] == NULL && command_id && strlen(input) >= 2)
            {
                fprintf(stderr, "%s", MSG_SYNTAX_ERROR);
                fflush(stderr);
                parser_ignore_line();
                return;
            }
            
            // zero memory
            memset(&command->input, 0, sizeof(Stream));
            memset(&command->output, 0, sizeof(Stream));
            command->argc = 0;
            for (int i = 0; command->argv[i] != NULL; i++)
                ++command->argc; 
            
            command->input.type  = STREAM_STDIN;
            command->output.type = STREAM_STDOUT;

            if (parser_has_next_command())
                command->output.type = STREAM_PIPE;
            if (parser_has_prev_command())
            {
                Command* prev = parser_get_pipeline()[g_parser.command - 1];
                if (prev->output.type == STREAM_PIPE)
                    command->input.type  = STREAM_PIPE;
                else
                    command->input.type  = STREAM_STDIN;
            }
            
            for (int i = 0; command->redirs[i] != NULL; i++)
            {
                Redirection* r = command->redirs[i];

                if (IS_RAPPEND(r->flags))
                {
                    command->output.type = STREAM_FILE_APPEND;
                    command->output.file = r->filename;
                } 
                else if (IS_ROUT(r->flags))
                {
                    command->output.type = STREAM_FILE;
                    command->output.file = r->filename;
                }
                else if (IS_RIN(r->flags))
                {
                    command->input.type = STREAM_FILE;
                    command->input.file = r->filename;
                }
            }
            parser_next_command();
            ++command_id;
        }
        parser_next_pipeline();
    }
    g_parser.pipeline = 0;
    g_parser.command = 0;
}

Pipeline parser_get_pipeline()
{
    if (g_parser.parsed_line)
        return g_parser.parsed_line->pipelines[g_parser.pipeline];
    return NULL;
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

void parser_ignore_line()
{
    while (parser_get_pipeline())
        parser_next_pipeline();
}

void parser_next_command()
{
    if (parser_get_command())
        g_parser.command++;
}

bool parser_has_next_command()
{
    return parser_get_command() && parser_get_pipeline()[g_parser.command + 1];
}

bool parser_has_prev_command()
{
    return g_parser.command && parser_get_pipeline()[g_parser.command - 1];
}

bool parser_in_background()
{
    return g_parser.background;
}
