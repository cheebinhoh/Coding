# Copyright © 2021-2023 Chee Bin HOH. All rights reserved.
#
# Old good makefile to help manage compilation.
#
# TODO: move into automake, libtool and autoconf for more scalability and proper dependency.

all : exception-vs-std-optional.out lambda-functor.out lOrRValue.out pointer-to-member.out ring-buffer-for-multiple-writers-n-one-reader.out \
	singleton-design-pattern.out smart-pointer-with-object-graph.out std-array.out std-chrono.out \
	std-containers-and-algorithms.out std-function-object.out std-future-async.out std-future-packaged-task.out \
	std-future-promise.out std-map.out std-pair.out std-priority-queue.out std-reference-in-std-container.out \
	std-thread-with-multiple-readers-n-one-writer.out std-thread-with-multiple-worker-threads.out std-tuple.out \
	test-template-class.out 

exception-vs-std-optional.out : exception-vs-std-optional.cpp
	g++ -std=c++17 -o $@ exception-vs-std-optional.cpp

lOrRValue.out : lOrRValue.cpp
	g++ -std=c++11 -o $@ lOrRValue.cpp

lambda-functor.out : lambda-functor.cpp
	g++ -std=c++14 -o $@ lambda-functor.cpp

pointer-to-member.out : pointer-to-member.cpp
	g++ -std=c++20 -o $@ pointer-to-member.cpp

singleton-design-pattern.out : singleton-design-pattern.cpp
	g++ -std=c++14 -o $@ singleton-design-pattern.cpp

smart-pointer-with-object-graph.out : smart-pointer-with-object-graph.cpp
	g++ -std=c++14 -o $@ smart-pointer-with-object-graph.cpp

ring-buffer-for-multiple-writers-n-one-reader.out : ring-buffer-for-multiple-writers-n-one-reader.cpp
	g++ -std=c++14 -o $@ ring-buffer-for-multiple-writers-n-one-reader.cpp

std-array.out : std-array.cpp
	g++ -std=c++14 -o $@ std-array.cpp

std-chrono.out : std-chrono.cpp
	g++ -std=c++14 -o $@ std-chrono.cpp

std-containers-and-algorithms.out : std-containers-and-algorithms.cpp
	g++ -std=c++17 -o $@ std-containers-and-algorithms.cpp

std-function-object.out : std-function-object.cpp
	g++ -std=c++17 -o $@ std-function-object.cpp

std-future-async.out : std-future-async.cpp
	g++ -std=c++17 -o $@ std-future-async.cpp

std-future-packaged-task.out : std-future-packaged-task.cpp
	g++ -std=c++17 -o $@ std-future-packaged-task.cpp

std-future-promise.out : std-future-promise.cpp
	g++ -std=c++17 -o $@ std-future-promise.cpp

std-map.out : std-map.cpp
	g++ -std=c++14 -o $@ std-map.cpp

std-pair.out : std-pair.cpp
	g++ -std=c++14 -o $@ std-pair.cpp

std-priority-queue.out : std-priority-queue.cpp
	g++ -std=c++14 -o $@ std-priority-queue.cpp

std-reference-in-std-container.out : std-reference-in-std-container.cpp
	g++ -std=c++14 -o $@ std-reference-in-std-container.cpp

std-thread-with-multiple-worker-threads.out : std-thread-with-multiple-worker-threads.cpp
	g++ -std=c++14 -o $@ std-thread-with-multiple-worker-threads.cpp

std-thread-with-multiple-readers-n-one-writer.out : std-thread-with-multiple-readers-n-one-writer.cpp
	g++ -std=c++14 -o $@ std-thread-with-multiple-readers-n-one-writer.cpp

std-tuple.out : std-tuple.cpp
	g++ -std=c++14 -o $@ std-tuple.cpp

test-template-class.out : test-template-class.cpp template-class.cpp
	g++ -std=c++14 -o $@ test-template-class.cpp template-class.cpp

# miscallenous
clean:
	rm -f *.out *.o lib*.a lib*.so

test: all
