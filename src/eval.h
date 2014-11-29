#ifndef _EVAL_H_
#define _EVAL_H_
#include "config.h"
#include "parser-core/parser.h"
#define EVAL_ERROR_ENOENT "no such file or directory\n"
#define EVAL_ERROR_EACCES "permission denied\n"
#define EVAL_ERROR_EXEC "exec error\n"
#define EVAL_ERROR_UHNDLD "unhandled exception\n"
#define EVAL_MAX_CHILDREN MAX_LINE_LENGTH

/* evaluates the command */
int eval(Command* cmd, int fd[2], bool background);

#endif
