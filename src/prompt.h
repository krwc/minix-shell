#ifndef _PROMPT_H_
#define _PROMPT_H_
#include <sys/stat.h>
#include <unistd.h>
#define COLOR_BLACK  "\033[1;30m"
#define COLOR_CYAN   "\033[0;36m"
#define COLOR_BLUE   "\033[0;34m"
#define COLOR_GREEN  "\033[0;32m"
#define COLOR_RED    "\033[0;31m"
#define COLOR_PURPLE "\033[0;35m"
#define COLOR_NONE   "\033[0m"

// Prompt modifiers
#define PROMPT_MOD_HOST 'h'
#define PROMPT_MOD_USER 'u'
#define PROMPT_MOD_MODE '$'
#define PROMPT_MOD_WORKDIR 'w'

// Default prompt
#define PROMPT_DEFAULT "\\u@\\h \\w \\$ "

// Maximal prompt length
#define MAX_PROMPT_LENGTH 2048

void prompt_append_char(char character, size_t* offset);
void prompt_append_string(const char* string, size_t* offset);


char prompt_buffer[MAX_PROMPT_LENGTH];

void print_prompt();

#endif
