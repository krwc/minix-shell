CC=clang
CFLAGS=-g

SOURCES=src/main.c src/eval.c src/message.c src/prompt.c src/input.c src/builtins.c
OBJS=$(SOURCES:.c=.o)
LIBS=src/parser-core/parser-core.a

all: mshell
clean: 
	rm mshell src/*.o; \
        make -C src/parser-core clean

src/parser-core/parser-core.a:
	$(MAKE) -C src/parser-core

mshell: $(OBJS) $(LIBS)
	$(CC) $(CFLAGS) $(OBJS) $(LIBS) -o $@

src/%.o: %.c
	$(CC) $(CFLAGS) -c $<

