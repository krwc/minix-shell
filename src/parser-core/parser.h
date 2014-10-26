#ifndef _PARSER_H_
#define _PARSER_H_
#include <stdio.h>
#include "siparse.h"

typedef struct Parser
{
    Line* parsed_line;
    /* indexes of currently watched pipeline and command */
    int pipeline;
    int command;
} Parser;

void parser_parse_line(const char* input);

/* switches to the next pipeline */
void parser_next_pipeline();
/* switches to the next command in current pipeline */
void parser_next_command();
/* returns current pipeline */
Pipeline parser_get_pipeline();
/* returns current command */
Command* parser_get_command();

#endif
