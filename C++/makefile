# old good makefile to help compiling
#
# TODO: move into automake, libtool and autoconf for more scalability and proper dependency.

all : smart-pointer-with-object-graph.out std-reference-in-std-container.out \
	exception-vs-std-optional.out singleton-design-pattern.out lambda-functor.out \
	std-array.out std-pair.out std-tuple.out std-containers-and-algorithms.out

lambda-functor.out : lambda-functor.cpp
	g++ -std=c++14 -o $@ lambda-functor.cpp

exception-vs-std-optional.out : exception-vs-std-optional.cpp
	g++ -std=c++17 -o $@ exception-vs-std-optional.cpp

singleton-design-pattern.out : singleton-design-pattern.cpp
	g++ -std=c++14 -o $@ singleton-design-pattern.cpp

smart-pointer-with-object-graph.out : smart-pointer-with-object-graph.cpp
	g++ -std=c++14 -o $@ smart-pointer-with-object-graph.cpp

std-array.out : std-array.cpp
	g++ -std=c++14 -o $@ std-array.cpp

std-pair.out : std-pair.cpp
	g++ -std=c++14 -o $@ std-pair.cpp

std-tuple.out : std-tuple.cpp
	g++ -std=c++14 -o $@ std-tuple.cpp

std-reference-in-std-container.out : std-reference-in-std-container.cpp
	g++ -std=c++14 -o $@ std-reference-in-std-container.cpp

std-containers-and-algorithms.out : std-containers-and-algorithms.cpp
	g++ -std=c++14 -o $@ std-containers-and-algorithms.cpp

# miscallenous
clean:
	rm -f *.out *.o

test: all
	@echo "Done."
