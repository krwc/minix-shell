
#define MAX_COMMANDS (MAX_LINE_LENGTH / 2 + 1)
#define MAX_PIPELINES MAX_COMMANDS
#define MAX_ARGS MAX_COMMANDS
#define MAX_REDIRS MAX_COMMANDS

void printCommand(Command *com);
void resetutils(void);

/*
 * buffer for string from the parsed line
 */

char* copytobuffer(const char*, const short);
void resetbuffer(void);

/*
 * buffer for args
 * each argv is NULL terminated substring of the buffer
 */

char** appendtoargv(char*);
char** closeargv(void);
void resetargvs(void);

/*
 * buffer for Commands
 */
Command* nextCommand(void);
void resetCommands(void);

/*
 * buffer for Redirections
 */
Redirection* nextredir(void);
void resetredirs(void);
Redirection** appendtoredirseq(Redirection*);

Redirection** closeredirseq(void);
void resetredirseqs(void);

Pipeline appendtoPipeline(Command*);
Pipeline closePipeline(void);
void resetPipelines(void);

PipelineList appendtoPipelineList(Command**);
PipelineList closePipelineList(void);
void resetPipelineLists(void);

