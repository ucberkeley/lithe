SHELL = '/bin/sh'

all: 
	make -C spmd
	make -C hierarchy
	make -C tls
	make -C barrier
	make -C futures

clean:
	make clean -C spmd
	make clean -C hierarchy
	make clean -C tls
	make clean -C barrier
	make clean -C futures

.PHONY: all clean
