#ifndef _MESSAGE_H_
#define _MESSAGE_H_
#include "config.h"
#include <string.h>
#include <stdio.h>
#define MSG_SYNTAX_ERROR "Syntax error.\n"

void message_error(const char* program, const char* message);
void message(FILE* output, const char* message);

#endif
