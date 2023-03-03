# old good makefile to help compiling
#
# TODO: move into automake, libtool and autoconf for more scalability and proper dependency.

all : smart-pointer-4-object-graph.out

smart-pointer-4-object-graph.out : smart-pointer-4-object-graph.cpp
	g++ -std=c++14 -o $@ smart-pointer-4-object-graph.cpp

# miscallenous
clean:
	rm -f *.out *.o

test: all
	@echo "Done."
