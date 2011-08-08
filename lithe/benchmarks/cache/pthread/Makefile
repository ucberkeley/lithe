# Simple makefile for pthread version of benchmark.

FATAL := ~/fatal/fatal.h

PTHREAD_NP := ~/pthread_np/libpthread_np.so

# Confirm we have FATAL and PTHREAD_NP (in lieu of using 'configure').
ifneq (0, $(shell test -e $(FATAL) ; echo $$?))
$(error missing fatal.h)
else
FATAL := $(dir $(wildcard $(FATAL)))
endif

ifneq (0, $(shell test -e $(PTHREAD_NP) ; echo $$?))
$(error missing libpthread_np.so)
else
PTHREAD_NP := $(dir $(wildcard $(PTHREAD_NP)))
endif

default: all

main: main.c
	gcc -O3 -std=gnu99 -I.. -I$(FATAL) -I$(PTHREAD_NP) -L$(PTHREAD_NP) -lpthread_np -lpthread -o main main.c

all: main

clean:
	rm -f main