CC=clang
CFLAGS=

SOURCES=main.c eval.c message.c prompt.c input.c builtins.c signal.c
OBJS=$(SOURCES:.c=.o)
LIBS=parser-core/parser-core.a

all: mshell
clean: 
	rm mshell *.o; \
        make -C parser-core clean

parser-core/parser-core.a:
	$(MAKE) -C parser-core

mshell: $(OBJS) $(LIBS)
	$(CC) $(CFLAGS) $(OBJS) $(LIBS) -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $<

