#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include <config.h>
#include "siparse.h"
#include "siparseutils.h"

void resetutils(void)
{
    resetbuffer();
    resetargvs();
    resetCommands();
    resetredirs();
    resetredirseqs();
    resetPipelines();
    resetPipelineLists();
}

/*
 * buffer for string from the parsed line
 */
static char linebuffer[MAX_LINE_LENGTH + 1];
static const char* linebufferend = linebuffer + MAX_LINE_LENGTH;
static char* bufptr = linebuffer;

/*
 * name is 0 terminated string
 * length include terminating 0
 */
char* copytobuffer(const char* name, const short length)
{
    char* saved;
    int copied;

    saved = bufptr;
    bufptr += length;
    if (bufptr > linebufferend)
        return NULL;
    strncpy(saved, name, length);
    return saved;
}

void resetbuffer(void) { bufptr = linebuffer; }

/*
 * buffer for args
 * each argv is NULL terminated substring of the buffer
 */
static char* argvs[MAX_ARGS * 2];
static char** nextarg = argvs;
static char** currentargv = argvs;

char** appendtoargv(char* arg)
{
    *nextarg = arg;
    nextarg++;
    return currentargv;
}

char** closeargv(void)
{
    char** oldstart;

    appendtoargv(NULL);
    oldstart = currentargv;
    currentargv = nextarg;
    return oldstart;
}

void resetargvs(void)
{
    currentargv = argvs;
    nextarg = currentargv;
}

/*
 * buffer for Commands
 */
static Command Commandsbuf[MAX_COMMANDS + 1];
static Command* nextcom = Commandsbuf;

Command* nextCommand(void)
{
    Command* curr;

    curr = nextcom;
    nextcom++;
    //	nextcom->argv=NULL;
    return curr;
}

void resetCommands(void) { nextcom = Commandsbuf; }

/*
 * buffer for Redirections
 */
static Redirection redirsbuf[MAX_REDIRS];
static Redirection* nextred;

Redirection* nextredir(void) { return (nextred++); }

void resetredirs(void) { nextred = redirsbuf; };

static Redirection* redirseqbuf[MAX_REDIRS * 2];
static Redirection** currentredirseq;
static Redirection** currentredirseqstart;

Redirection** appendtoredirseq(Redirection* redir)
{

    *currentredirseq = redir;
    currentredirseq++;

    return currentredirseqstart;
}

Redirection** closeredirseq(void)
{
    Redirection** oldstart;

    oldstart = appendtoredirseq(NULL);
    currentredirseqstart = currentredirseq;

    return oldstart;
};

void resetredirseqs(void)
{
    currentredirseqstart = redirseqbuf;
    currentredirseq = redirseqbuf;
}

/*
 * Pipelines buffer
 */

static Command* Pipelinesbuf[MAX_COMMANDS * 2];
static Command** currentPipelinecomm;
static Command** currentPipelinestart;

Pipeline appendtoPipeline(Command* comm)
{

    *currentPipelinecomm = comm;
    currentPipelinecomm++;

    return currentPipelinestart;
}

Pipeline closePipeline(void)
{
    Command** oldstart;

    oldstart = appendtoPipeline(NULL);
    currentPipelinestart = currentPipelinecomm;

    return oldstart;
};

void resetPipelines(void)
{
    currentPipelinecomm = Pipelinesbuf;
    currentPipelinestart = Pipelinesbuf;
}

/*
 * Pipelinesseq buffer
 */

static Pipeline PipelineListsbuf[MAX_PIPELINES * 2];
static Pipeline* currentPipeline;
static Pipeline* currentPipelineListstart;

PipelineList appendtoPipelineList(Command** Pipeline)
{

    *currentPipeline = Pipeline;
    currentPipeline++;

    return currentPipelineListstart;
}

PipelineList closePipelineList(void)
{
    PipelineList oldstart;

    oldstart = appendtoPipelineList(NULL);
    currentPipelineListstart = currentPipeline;

    return oldstart;
};

void resetPipelineLists(void)
{
    currentPipeline = PipelineListsbuf;
    currentPipelineListstart = PipelineListsbuf;
}

// printing
void printCommand(Command *com){
        char **arg;
        Redirection ** red;


        printf("argv:\n");
        for (arg = com->argv; *arg; arg++){
                printf(":%s:", *arg);
        }
        printf("\n");

        printf("Redirections:\n");
        for (red = com->redirs; *red; red++){
                printf("filename:%s: flags: %d\n", (*red)->filename, (*red)->flags);
        }
        printf("\n");
}
