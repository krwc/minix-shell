#ifndef _SIPARSE_H_
#define _SIPARSE_H_

/*
 * Redirection flags
 */
#define RIN     (1 << 0)
#define ROUT    (1 << 1)
#define RAPPEND (1 << 2)

#define RTMASK        (RIN | ROUT | RAPPEND)
#define IS_RIN(x)     (((x)&RTMASK) == RIN)
#define IS_ROUT(x)    (((x)&RTMASK) == ROUT)
#define IS_RAPPEND(x) (((x)&RTMASK) == (ROUT | RAPPEND))

typedef struct Redirection
{
    char *filename;
    int flags;
} Redirection;

typedef enum StreamType
{
    STREAM_STDIN,
    STREAM_STDOUT,
    STREAM_PIPE_IN,
    STREAM_PIPE_OUT,
    STREAM_FILE,
    STREAM_FILE_APPEND
} StreamType;

typedef struct Stream
{
    StreamType type;
    char* file;
} Stream;

typedef struct Command 
{
    char** argv; 		/* NULL ended array of arguments */
    int argc;                   /* number of arguments passed */
    Stream input;
    Stream output;
    Redirection** redirs;	/* NULL ended array of pointers to Redirections */
} Command;  

/* NULL ended array of pointers to Commands */
typedef Command** Pipeline;

/* NULL ended array of PipeLines */
typedef Pipeline* PipelineList;

typedef struct Line 
{
    PipelineList pipelines;
    int flags;
} Line;

/*
 * flags for parsed Line
 */
#define LINBACKGROUND 1


/*
 * Parses given string containing sequence of PipeLines separated by ';'. 
 * Each PipeLine is a sequence of Commands separated by '|'.
 * Function returns a pointer to the static structure Line or NULL if meets a parse error.
 * All structures referenced from the result of the function are statically allocated and shall not be freed.
 * Passing a string longer than MAX_LINE_LENGHT may result in an unspecified behaviour.
 * Consecutive calls to the function destroy the content of previously returned structures.
 */
Line* parseline(char *);

#endif /* !_SIPARSE_H_ */
