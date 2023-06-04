# Copyright © 2021-2023 Chee Bin HOH. All rights reserved.
#
# Old good makefile to help manage compilation.
#
# TODO: move into automake, libtool and autoconf for more scalability and proper dependency.

all : copy.out copystdio.out generateNumbers.out helloprocess.out listdir.out \
	readGeneratedNumbers.out 

copy.out : copy.c
	gcc -o $@ copy.c

copystdio.out : copystdio.c
	gcc -o $@ copystdio.c

helloprocess.out : helloprocess.c
	gcc -o $@ helloprocess.c

listdir.out : listdir.c
	gcc -o $@ listdir.c

generateNumbers.out : generateNumbers.c
	gcc -o $@ generateNumbers.c

readGeneratedNumbers.out : readGeneratedNumbers.c
	gcc -o $@ readGeneratedNumbers.c

# miscallenous
clean:
	rm -f *.out *.o lib*.a

test: all
