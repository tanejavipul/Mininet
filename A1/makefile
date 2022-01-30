SHELL = /bin/bash
FLAGS = -Wall -Werror -std=c99
.PHONY: all clean

# https://stackoverflow.com/questions/653807/determining-c-executable-name
# https://stackoverflow.com/questions/3220277/what-do-the-makefile-symbols-and-mean

%.o: %.c
	gcc ${CFLAGS} -c $<

SimpleServer: SimpleServer.o Helper.o
	gcc ${FLAGS} -o $@ $<

PersistentServer: PersistentServer.o PersistentHelper.o
	gcc ${FLAGS} -o $@ $<

PipelinedServer: PipelinedServer.c
	gcc ${FLAGS} -o $@ $<

test: Test.o Helper.o
	gcc ${FLAGS} -o $@ $<

all: clean SimpleServer PersistentServer PipelinedServer test

clean:
	rm -f SimpleServer PersistentServer PipelinedServer *.o test







# DONT TOUCH
#
#.PHONY: all test_vs clean
#all: test_vs
#
#validate_sin: validate_sin.o sin_helpers.o
#	gcc ${FLAGS} -o $@ $^
#
#%.o: %.c
#	gcc ${FLAGS} -c $<
#
#test_vs: validate_sin
#	@test_vs_output=`./validate_sin 810620716`; \
#	if [ "$$test_vs_output" == "Valid SIN" ]; then \
#		echo Compiled and sanity check passed; \
#	else \
#		echo Failed sanity check; \
#	fi
#
#clean:
#	rm -f *.o validate_sin