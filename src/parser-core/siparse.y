%{
#include "siparse.h"
#include "siparseutils.h"
#include <stdio.h>
    
extern int yyleng;
int yylex(void);
void yyerror(char *);
  
void switchinputbuftostring(const char *);
void freestringinputbuf(void);

static Line parsed_line;
%}


%union {
	int flags;
	char *name;
	char **argv;
	Redirection *redir;
	Redirection **redirseq;
	Command *comm;
	Pipeline pipeln;
	PipelineList pipelnsq;
	Line* parsedln;
}

%token SSTRING
%token OAPPREDIR
%token COMMENT
%%
line: pipelineseq mamp mcomment mendl {
			parsed_line.pipelines = closePipelineList(); 
			parsed_line.flags = $<flags>2;
			$<parsedln>$ = &parsed_line;
		}
	;

mamp: 
	'&' { $<flags>$ = LINBACKGROUND; } 
	|	{ $<flags>$ = 0; }
	;

mendl:
	'\n'
	|
	;

mcomment:
          COMMENT
          |
          ;

pipelineseq:
	pipelineseq ';' prepipeline{
			$<pipelnsq>$ = appendtoPipelineList($<pipeln>3);
		}
	| prepipeline{
			$<pipelnsq>$ = appendtoPipelineList($<pipeln>1);
		}
	;

prepipeline:
	pipeline {
			closePipeline();
		}
	;

pipeline:
	pipeline '|' single {
			$<pipeln>$ = appendtoPipeline($<comm>3);
		}
	| single {
			$<pipeln>$ = appendtoPipeline($<comm>1);
		}
	;

single:
	allnames allredirs {
			if ($<argv>1==NULL) {
				$<comm>$ = NULL;	
			} else {
				Command* com= nextCommand();
				com->argv = $<argv>1;
				com->redirs = $<redirseq>2;
				$<comm>$ = com;
			}
		}
	;

allnames:
		names { $<argv>$ = closeargv(); }


allredirs:
		 redirs { $<redirseq>$ = closeredirseq(); }

names:
	names name {
			$<argv>$ = appendtoargv($<name>2);
		} 
	|	 
	;

name:	SSTRING {
			$<name>$ = copytobuffer(yyval.name, yyleng+1);
		};

redirs:
	redirs redir {
			$<redirseq>$ = appendtoredirseq($<redir>2);
		}
	|	{	$<redirseq>$ = NULL; };
	;

redir:
	redirIn
	| redirOut
	;

redirIn:
	'<' rname { $<redir>2->flags = RIN; $<redir>$=$<redir>2; }
	;

redirOut:
	OAPPREDIR rname 	{ $<redir>2->flags = ROUT | RAPPEND ; $<redir>$=$<redir>2; }
	| '>' rname	{ $<redir>2->flags = ROUT; $<redir>$=$<redir>2; }
	;

rname:
	 name {
			Redirection* red;

			red=nextredir();
			red->filename = $<name>1;
			$<redir>$= red;
		}

%%

void yyerror(char *s) {
}


Line* parseline(char *str)
{
	int parseresult;

	resetutils();
	switchinputbuftostring(str);
	parseresult = yyparse();
	freestringinputbuf();

	if (parseresult) return NULL;
	return &parsed_line;
}

