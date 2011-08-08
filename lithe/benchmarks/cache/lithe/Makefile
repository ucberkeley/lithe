# Simple makefile for lithe version of benchmark.

FATAL := ~/fatal/fatal.h

PTHREAD_NP := ~/pthread_np/libpthread_np.so

HT := ~/ht/libht.so

# Confirm we have FATAL, PTHREAD_NP, and HT (in lieu of using 'configure').
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

ifneq (0, $(shell test -e $(HT) ; echo $$?))
$(error missing libht.so)
else
HT := $(dir $(wildcard $(HT)))
endif

default: all

main: main.c
	gcc -O3 -std=gnu99 -I.. -I../../../include -I$(FATAL) -I$(HT) -L$(HT) -lht -L../../.. -llithe -L$(PTHREAD_NP) -lpthread_np -o main main.c

all: main

clean:
	rm -f main