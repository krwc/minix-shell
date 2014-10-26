CC=clang
CFLAGS=-g -std=c99
INCLUDES=-I../

all: siparseutils.o parser.o y.tab.o lex.yy.o
	ar rcs $(INSTALL_DIR) parser-core.a siparseutils.o parser.o lex.yy.o y.tab.o 

lex.yy.o: siparse.lex y.tab.o
	lex  siparse.lex
	$(CC) $(CFLAGS) $(INCLUDES) lex.yy.c -c

y.tab.o: siparse.y  $(HDEPS)
	yacc -d siparse.y
	$(CC) $(CFLAGS) $(INCLUDES) y.tab.c -c

parser.o: parser.c $(HDEPS)
	$(CC) $(CFLAGS) $(INCLUDES) -c $<
siparseutils.o: siparseutils.c $(HDEPS)
	$(CC) $(CFLAGS) $(INCLUDES) -c $<

clean:
	rm -f lex.yy.c y.tab.c y.tab.h parser-core.a  *.o
