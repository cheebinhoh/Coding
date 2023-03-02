# old good makefile to help compiling
#
# TODO: move into automake, libtool and autoconf for more scalability and proper dependency.

all : box.out copy.out test.out smart-pointer-4-object-graph.out

copy.out : copy.cpp
	g++ -std=c++14 -o $@ copy.cpp

box.out : box.cpp
	g++ -std=c++14 -o $@ box.cpp

test.out : test.cpp
	g++ -std=c++14 -o $@ test.cpp

smart-pointer-4-object-graph.out : smart-pointer-4-object-graph.cpp
	g++ -std=c++14 -o $@ smart-pointer-4-object-graph.cpp

# miscallenous
clean:
	rm -f *.out *.o

test: all
	@echo "Done."
