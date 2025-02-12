# Copyright © 2021-2025 Chee Bin HOH. All rights reserved.
#
# Old good makefile to help manage compilation.

all : libhal.so hal-test.out hal-test-teepipe.out hal-test-io.out

libhal.so : hal-async.hpp hal-buffer.hpp hal-limit-buffer.hpp hal-pipe.hpp hal-proc.cpp \
		hal-proc.hpp hal-teepipe.hpp hal.hpp
	g++ -std=c++17 -c -fPIC hal-proc.cpp
	g++ -std=c++17 hal-proc.o -shared -o libhal.so -lpthread

hal-test.out : hal-test.cpp libhal.so
	g++ -std=c++17 -o $@ hal-test.cpp -lpthread -L. -lhal

hal-test-teepipe.out : hal-test-teepipe.cpp libhal.so
	g++ -std=c++17 -o $@ hal-test-teepipe.cpp -L. -lhal

hal-test-io.out : hal-test-io.cpp libhal.so
	g++ -std=c++17 -o $@ hal-test-io.cpp hal-proc.cpp

# miscallenous
clean:
	rm -f *.out *.o lib*.a lib*.so

test: all
