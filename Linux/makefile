# old good makefile to help compiling
#
# TODO: move into automake, libtool and autoconf for more scalability and proper dependency.

all : listdir.out copy.out copystdio.out

copy.out : copy.c
	gcc -o $@ copy.c

listdir.out : listdir.c
	gcc -o $@ listdir.c

copystdio.out : copystdio.c
	gcc -o $@ copystdio.c

# miscallenous
clean:
	rm -f *.out *.o

test: all
	@echo
