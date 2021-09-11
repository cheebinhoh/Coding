# old good makefile to help compiling

all : btreepathsum.out btreebltraverse.out btreemaxlevel.out btreesymmetriccheck.out btreetraverse.out \
	btreebuild.out btreelca.out tree2btreebuild.out search-sort.out remove-c-comment.out \
 	string-utility.out calculator.out shrink-space.out trim-space.out replace-tab-with-space.out \
	syntax-validate-parenthese.out fold-line.out find2ndMaxNumber.out transformStr2Str.out \
	trafficdemand.out coding-test.out btreemirrorswap.out btreeidentical.out \
        btreerebalancing.out

btreepathsum.out : btreepathsum.c 
	gcc -o $@ btreepathsum.c 

btreebltraverse.out : btreebltraverse.c btree.h 
	gcc -o $@ btreebltraverse.c

btreemaxlevel.out : btreemaxlevel.c btree.h
	gcc -o $@ btreemaxlevel.c

btreesymmetriccheck.out : btreesymmetriccheck.c btree.h
	gcc -o $@ btreesymmetriccheck.c

btreetraverse.out : btreetraverse.c btreetraverse-main.c btree.h btreetraverse.h
	gcc -o $@ btreetraverse.c btreetraverse-main.c

btreebuild.out : btreebuild.c btreetraverse.c btree.h btreetraverse.h
	gcc -o $@ btreebuild.c btreetraverse.c 

btreelca.out : btreelca.c btreetraverse.c btree.h btreetraverse.h
	gcc -o $@ btreelca.c btreetraverse.c 

tree2btreebuild.out : tree2btreebuild.c btreetraverse.c btree.h btree.c btreetraverse.h 
	gcc -o $@ tree2btreebuild.c btreetraverse.c btree.c

search-sort.out : search-sort.c 
	gcc -o $@ search-sort.c 

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

coding-test.out : coding-test.c btree.c btree.h
	gcc -o $@ coding-test.c btree.c

btreemirrorswap.out : btreemirrorswap.c btree.c btree.h
	gcc -o $@ btreemirrorswap.c btree.c

btreeidentical.out : btreeidentical.c btree.c btree.h
	gcc -o $@ btreeidentical.c btree.c

btreerebalancing.out : btreerebalancing.c btree.c btree.h
	gcc -o $@ btreerebalancing.c btree.c


clean:
	rm -f *.out *.o


test:
	make all
	@echo
	./transformStr2Str.out -d > ./transformStr2Str_result.txt
