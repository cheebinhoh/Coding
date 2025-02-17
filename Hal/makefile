# Copyright © 2021-2025 Chee Bin HOH. All rights reserved.
#
# Old good makefile to help manage compilation.

all : libhal.so hal.out hal-test.out hal-test-async.out hal-test-buffer.out hal-test-io.out \
	 hal-test-pipe.out hal-test-pub-sub.out hal-test-singleton.out hal-test-teepipe.out

libhal.so : hal-async.hpp hal-buffer.hpp hal-io.hpp hal-limit-buffer.hpp hal-pipe.hpp hal-proc.cpp \
		hal-proc.hpp hal-pub-sub.hpp hal-singleton.hpp hal-teepipe.hpp hal.hpp
	g++ -std=c++17 -c -fPIC hal-proc.cpp
	g++ -std=c++17 hal-proc.o -shared -o libhal.so -lpthread

hal.out : hal.cpp libhal.so
	g++ -std=c++17 -o $@ hal.cpp -lpthread -L. -lhal

hal-test.out : hal-test.cpp libhal.so
	g++ -std=c++17 -o $@ hal-test.cpp -lpthread -L. -lhal

hal-test-async.out : hal-test-async.cpp libhal.so
	g++ -std=c++17 -o $@ hal-test-async.cpp -lpthread -L. -lhal

hal-test-buffer.out : hal-test-buffer.cpp libhal.so
	g++ -std=c++17 -o $@ hal-test-buffer.cpp -lpthread -L. -lhal

hal-test-io.out : hal-test-io.cpp libhal.so
	g++ -std=c++17 -o $@ hal-test-io.cpp hal-proc.cpp

hal-test-pipe.out : hal-test-pipe.cpp libhal.so
	g++ -std=c++17 -o $@ hal-test-pipe.cpp -L. -lhal

hal-test-pub-sub.out : hal-test-pub-sub.cpp libhal.so
	g++ -std=c++17 -o $@ hal-test-pub-sub.cpp -L. -lhal

hal-test-singleton.out : hal-test-singleton.cpp libhal.so
	g++ -std=c++17 -o $@ hal-test-singleton.cpp -L. -lhal

hal-test-teepipe.out : hal-test-teepipe.cpp libhal.so
	g++ -std=c++17 -o $@ hal-test-teepipe.cpp -L. -lhal

# miscallenous
clean:
	rm -f *.out *.o lib*.a lib*.so

test: all
