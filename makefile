# Copyright © 2021-2023 Chee Bin HOH. All rights reserved.
#
# Old good makefile to help manage compilation.
#
# TODO: move into automake, libtool and autoconf for more scalability and proper dependency.

all : bigo-sample.out btree-test.out btreebltraverse.out btreebuild.out btreeidentical.out btreeisbalanced.out btreelca.out \
	btreemaxminlevel.out btreemaxnodeinlevel.out btreemaxpathsum.out btreemaxsumpathbetween2leaves.out \
	btreemaxsumpathtoleaf.out btreemirrorswap.out btreepathsum.out btreerebalancing.out btreesubtree.out \
	btreesymmetriccheck.out btreetraverse.out btreeverticalsum.out calculator.out cntdown.out coding-test-2.out \
	coding-test.out dgraph-test.out find2ndMaxNumber.out fold-line.out libbtree.so libdgraph.a libllist.a \
	libsearch-sort.a llist-test.out remove-c-comment.out replace-tab-with-space.out search-sort.out \
	shrink-space.out string-utility.out syntax-validate-parenthese.out trafficdemand.out transformStr2Str.out \
	tree2btreebuild.out trim-space.out
	@echo
	cd C++ && make all
	@echo
	cd Linux && make all
	@echo
	cd Hal && make all

# libraries
libbtree.so : btree-internal.h btree.h btree.c avlbstree.h avlbstree.c llist.h llist-internal.h llist.c
	gcc -c -fPIC btree.c avlbstree.c llist.c
	gcc btree.o avlbstree.o llist.o -shared -o libbtree.so

#static library
#libbtree.a : btree-internal.h btree.h btree.c avlbstree.h avlbstree.c llist.h llist-internal.h llist.c
#	gcc -c btree.c avlbstree.c
#	gcc -c llist.c
#	ar -rc libbtree.a btree.o avlbstree.o llist.o

libdgraph.a : dgraph.h dgraph-internal.h dgraph.c
	gcc -c dgraph.c
	ar -rc libdgraph.a dgraph.o

libllist.a : llist.c llist.h llist-internal.h
	gcc -c llist.c
	ar -rc libllist.a llist.o

libsearch-sort.a : search-sort.c search-sort.h
	gcc -c search-sort.c
	ar -rc libsearch-sort.a search-sort.o

# programs
bigo-sample.out : bigo-sample.c
	gcc -o $@ bigo-sample.c

btreebltraverse.out : btreebltraverse.c btree.h libbtree.so
	gcc -o $@ btreebltraverse.c -L. -lbtree

btreebuild.out : btreebuild.c btreetraverse.c btree.h btreetraverse.h libbtree.so
	gcc -o $@ btreebuild.c btreetraverse.c -L. -lbtree

btreeidentical.out : btreeidentical.c btree.h libbtree.so
	gcc -o $@ btreeidentical.c -L. -lbtree

btreeisbalanced.out : btreeisbalanced.c btree.h libbtree.so
	gcc -o $@ btreeisbalanced.c -L. -lbtree

btreelca.out : btreelca.c btreetraverse.c btree.h btreetraverse.h libbtree.so
	gcc -o $@ btreelca.c btreetraverse.c -L. -lbtree

btreemaxminlevel.out : btreemaxminlevel.c btree.h libbtree.so
	gcc -o $@ btreemaxminlevel.c -L. -lbtree

btreemaxnodeinlevel.out : btreemaxnodeinlevel.c btree.h libbtree.so
	gcc -o $@ btreemaxnodeinlevel.c -L. -lbtree

btreemaxpathsum.out : btreemaxpathsum.c btree.h libbtree.so
	gcc -o $@ btreemaxpathsum.c -L. -lbtree

btreemaxsumpathbetween2leaves.out : btreemaxsumpathbetween2leaves.c libbtree.so btree.h
	gcc -o $@ btreemaxsumpathbetween2leaves.c -L. -lbtree

btreemaxsumpathtoleaf.out : btreemaxsumpathtoleaf.c libbtree.so btree.h
	gcc -o $@ btreemaxsumpathtoleaf.c -L. -lbtree

btreemirrorswap.out : btreemirrorswap.c btree.h libbtree.so
	gcc -o $@ btreemirrorswap.c -L. -lbtree

btreepathsum.out : btreepathsum.c btree.h libbtree.so
	gcc -o $@ btreepathsum.c -L. -lbtree

btreerebalancing.out : btreerebalancing.c libbtree.so btree.h
	gcc -o $@ btreerebalancing.c -L. -lbtree

btreesubtree.out : btreesubtree.c libbtree.so btree.h
	gcc -o $@ btreesubtree.c -L. -lbtree

btreesymmetriccheck.out : btreesymmetriccheck.c btree.h libbtree.so
	gcc -o $@ btreesymmetriccheck.c -L. -lbtree

btreetraverse.out : btreetraverse.c btreetraverse-main.c btree.h btreetraverse.h libbtree.so
	gcc -o $@ btreetraverse.c btreetraverse-main.c -L. -lbtree

btreeverticalsum.out : btreeverticalsum.c libbtree.so btree.h
	gcc -o $@ btreeverticalsum.c -L. -lbtree

btree-test.out : btree-test.c libbtree.so btree.h
	gcc -o $@ btree-test.c -L. -lbtree

calculator.out : calculator.c
	gcc -o $@ calculator.c

coding-test.out : coding-test.c btree.h libbtree.so search-sort.h libsearch-sort.a
	gcc -o $@ coding-test.c -L. -lbtree -lsearch-sort

coding-test-2.out : coding-test-2.c libbtree.so btree.h libllist.a llist.h search-sort.h libsearch-sort.a
	gcc -o $@ coding-test-2.c -L. -lbtree -lllist -lsearch-sort

cntdown.out : cntdown.c
	gcc -o $@ cntdown.c

dgraph-test.out : dgraph-test.c libdgraph.a libllist.a
	gcc -o $@ dgraph-test.c -L. -ldgraph -lllist

find2ndMaxNumber.out : find2ndMaxNumber.c
	gcc -o $@ find2ndMaxNumber.c

fold-line.out : fold-line.c
	gcc -o $@ fold-line.c

llist-test.out : llist-test.c libllist.a
	gcc -o $@ llist-test.c -L. -lllist

remove-c-comment.out : remove-c-comment.c
	gcc -o $@ remove-c-comment.c

replace-tab-with-space.out : replace-tab-with-space.c
	gcc -o $@ replace-tab-with-space.c

search-sort.out : search-sort-main.c search-sort.h libsearch-sort.a
	gcc -o $@ search-sort-main.c -L. -lsearch-sort

string-utility.out : string-utility.c
	gcc -o $@ string-utility.c

shrink-space.out : shrink-space.c
	gcc -o $@ shrink-space.c

syntax-validate-parenthese.out : syntax-validate-parenthese.c
	gcc -o $@ syntax-validate-parenthese.c

trafficdemand.out : trafficdemand.c
	gcc -o $@ trafficdemand.c

transformStr2Str.out : transformStr2Str.c
	gcc -o $@ transformStr2Str.c

tree2btreebuild.out : tree2btreebuild.c btreetraverse.c btree.h btreetraverse.h libbtree.so
	gcc -o $@ tree2btreebuild.c btreetraverse.c -L. -lbtree

trim-space.out : trim-space.c
	gcc -o $@ trim-space.c

# miscallenous
clean:
	rm -f *.out *.o lib*.a
	@echo
	cd C++ && make clean
	@echo
	cd Linux && make clean
	@echo
	cd Hal && make clean

test: all
	@echo
	./transformStr2Str.out -d | diff - ./transformStr2Str_result.txt
	./coding-test.out | diff - ./coding-test_result.txt
	./coding-test-2.out | diff - ./coding-test-2_result.txt
	for file in `echo *tree*.out | sort`; do echo "run $${file}"; echo; ./$${file} ; done | diff - ./btree_result.txt
