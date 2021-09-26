# old good makefile to help compiling
# 
# TODO: move into automake, libtool and autoconf for more scalability and proper dependency.

all : libbtree.a libllist.a libsearch-sort.a libdgraph.a btreepathsum.out btreebltraverse.out btreemaxminlevel.out \
        btreesymmetriccheck.out btreetraverse.out btreemirrorswap.out btreeidentical.out \
	btreebuild.out btreelca.out tree2btreebuild.out btreerebalancing.out btreemaxpathsum.out \
        btreeisbalanced.out btreemaxnodeinlevel.out btreeverticalsum.out btreemaxsumpathtoleaf.out \
        btreemaxsumpathbetween2leaves.out btreesubtree.out btree-test.out \
        search-sort.out remove-c-comment.out string-utility.out calculator.out shrink-space.out \
        trim-space.out replace-tab-with-space.out syntax-validate-parenthese.out fold-line.out \
        find2ndMaxNumber.out transformStr2Str.out trafficdemand.out coding-test.out \
        coding-test-2.out llist-test.out dgraph-test.out


# libraries
libbtree.a : btree-internal.h btree.h btree.c avlbstree.h avlbstree.c llist.h llist-internal.h llist.c
	gcc -c btree.c avlbstree.c
	gcc -c llist.c 
	ar -rc libbtree.a btree.o avlbstree.o llist.o

libllist.a : llist.c llist.h llist-internal.h
	gcc -c llist.c
	ar -rc libllist.a llist.o

libsearch-sort.a : search-sort.c search-sort.h
	gcc -c search-sort.c
	ar -rc libsearch-sort.a search-sort.o

libdgraph.a : dgraph.h dgraph-internal.h dgraph.c
	gcc -c dgraph.c
	ar -rc libdgraph.a dgraph.o

# programs
btreeisbalanced.out : btreeisbalanced.c btree.h libbtree.a
	gcc -o $@ btreeisbalanced.c -L. -lbtree

btreepathsum.out : btreepathsum.c btree.h libbtree.a 
	gcc -o $@ btreepathsum.c -L. -lbtree 

btreemaxpathsum.out : btreemaxpathsum.c btree.h libbtree.a
	gcc -o $@ btreemaxpathsum.c -L. -lbtree 

btreebltraverse.out : btreebltraverse.c btree.h libbtree.a 
	gcc -o $@ btreebltraverse.c -L. -lbtree 

btreemaxminlevel.out : btreemaxminlevel.c btree.h libbtree.a
	gcc -o $@ btreemaxminlevel.c -L. -lbtree

btreemaxnodeinlevel.out : btreemaxnodeinlevel.c btree.h libbtree.a
	gcc -o $@ btreemaxnodeinlevel.c -L. -lbtree

btreesymmetriccheck.out : btreesymmetriccheck.c btree.h libbtree.a
	gcc -o $@ btreesymmetriccheck.c -L. -lbtree

btreetraverse.out : btreetraverse.c btreetraverse-main.c btree.h btreetraverse.h libbtree.a
	gcc -o $@ btreetraverse.c btreetraverse-main.c -L. -lbtree

btreebuild.out : btreebuild.c btreetraverse.c btree.h btreetraverse.h libbtree.a
	gcc -o $@ btreebuild.c btreetraverse.c -L. -lbtree 

btreelca.out : btreelca.c btreetraverse.c btree.h btreetraverse.h libbtree.a
	gcc -o $@ btreelca.c btreetraverse.c -L. -lbtree

tree2btreebuild.out : tree2btreebuild.c btreetraverse.c btree.h btreetraverse.h libbtree.a
	gcc -o $@ tree2btreebuild.c btreetraverse.c -L. -lbtree

search-sort.out : search-sort-main.c search-sort.h libsearch-sort.a
	gcc -o $@ search-sort-main.c -L. -lsearch-sort

remove-c-comment.out : remove-c-comment.c
	gcc -o $@ remove-c-comment.c

string-utility.out : string-utility.c
	gcc -o $@ string-utility.c

calculator.out : calculator.c
	gcc -o $@ calculator.c

shrink-space.out : shrink-space.c
	gcc -o $@ shrink-space.c

trim-space.out : trim-space.c
	gcc -o $@ trim-space.c 

replace-tab-with-space.out : replace-tab-with-space.c 
	gcc -o $@ replace-tab-with-space.c 

syntax-validate-parenthese.out : syntax-validate-parenthese.c
	gcc -o $@ syntax-validate-parenthese.c

fold-line.out : fold-line.c
	gcc -o $@ fold-line.c

find2ndMaxNumber.out : find2ndMaxNumber.c
	gcc -o $@ find2ndMaxNumber.c

transformStr2Str.out : transformStr2Str.c
	gcc -o $@ transformStr2Str.c

trafficdemand.out : trafficdemand.c
	gcc -o $@ trafficdemand.c

coding-test.out : coding-test.c btree.h libbtree.a search-sort.h libsearch-sort.a
	gcc -o $@ coding-test.c -L. -lbtree -lsearch-sort

btreemirrorswap.out : btreemirrorswap.c btree.h libbtree.a
	gcc -o $@ btreemirrorswap.c -L. -lbtree

btreeidentical.out : btreeidentical.c btree.h libbtree.a
	gcc -o $@ btreeidentical.c -L. -lbtree

btreerebalancing.out : btreerebalancing.c libbtree.a btree.h
	gcc -o $@ btreerebalancing.c -L. -lbtree

coding-test-2.out : coding-test-2.c libbtree.a btree.h libllist.a llist.h search-sort.h libsearch-sort.a
	gcc -o $@ coding-test-2.c -L. -lbtree -lllist -lsearch-sort

llist-test.out : llist-test.c libllist.a
	gcc -o $@ llist-test.c -L. -lllist

btreeverticalsum.out : btreeverticalsum.c libbtree.a btree.h
	gcc -o $@ btreeverticalsum.c -L. -lbtree

btreemaxsumpathtoleaf.out : btreemaxsumpathtoleaf.c libbtree.a btree.h
	gcc -o $@ btreemaxsumpathtoleaf.c -L. -lbtree

btreemaxsumpathbetween2leaves.out : btreemaxsumpathbetween2leaves.c libbtree.a btree.h
	gcc -o $@ btreemaxsumpathbetween2leaves.c -L. -lbtree

btreesubtree.out : btreesubtree.c libbtree.a btree.h
	gcc -o $@ btreesubtree.c -L. -lbtree

btree-test.out : btree-test.c libbtree.a btree.h
	gcc -o $@ btree-test.c -L. -lbtree

dgraph-test.out : dgraph-test.c libdgraph.a libllist.a
	gcc -o $@ dgraph-test.c -L. -ldgraph -lllist 

# miscallenous
clean:
	rm -f *.out *.o lib*.a

test: all
	@echo
	./transformStr2Str.out -d | diff - ./transformStr2Str_result.txt
	./coding-test.out | diff - ./coding-test_result.txt 
	./coding-test-2.out | diff - ./coding-test-2_result.txt
	for file in `echo *tree*.out | sort`; do echo "run $${file}"; echo; ./$${file} ; done | diff - ./btree_result.txt
