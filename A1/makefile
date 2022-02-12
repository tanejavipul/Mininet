SHELL = /bin/bash
FLAGS = -Wall -Werror -std=c99 -pthread
.PHONY: all clean

# https://stackoverflow.com/questions/653807/determining-c-executable-name
# https://stackoverflow.com/questions/3220277/what-do-the-makefile-symbols-and-mean

%.o: %.c
	gcc ${CFLAGS} -c $<

SimpleServer: SimpleServer.o ServerHelper.o Parser.o
	gcc ${FLAGS} -o $@ $<

PersistentServer: PersistentServer.o ServerHelper.o Parser.o
	gcc ${FLAGS} -o $@ $<

PipelinedServer: PipelinedServer.c
	gcc ${FLAGS} -o $@ $<

all: clean SimpleServer PersistentServer PipelinedServer

clean:
	rm -f SimpleServer PersistentServer PipelinedServer *.o