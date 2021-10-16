# old good makefile to help compiling
#
# TODO: move into automake, libtool and autoconf for more scalability and proper dependency.

all : box.out copy.out test.out

copy.out : copy.cpp
	g++ -std=c++14 -o $@ copy.cpp

box.out : box.cpp
	g++ -std=c++14 -o $@ box.cpp

test.out : test.cpp
	g++ -std=c++14 -o $@ test.cpp

# miscallenous
clean:
	rm -f *.out *.o

test: all
	@echo
