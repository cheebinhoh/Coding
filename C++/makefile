# old good makefile to help compiling
#
# TODO: move into automake, libtool and autoconf for more scalability and proper dependency.

all : smart-pointer-with-object-graph.out std-reference-in-std-container.out

smart-pointer-with-object-graph.out : smart-pointer-with-object-graph.cpp
	g++ -std=c++14 -o $@ smart-pointer-with-object-graph.cpp

std-reference-in-std-container.out : std-reference-in-std-container.cpp
	g++ -std=c++14 -o $@ std-reference-in-std-container.cpp

# miscallenous
clean:
	rm -f *.out *.o

test: all
	@echo "Done."
