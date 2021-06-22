# all good makefile to help compiling

all : treepathsum.out treebltraverse.out treemaxlevel.out treesymmetriccheck.out treetraverse.out \
	treebuild.out treelca.out

treepathsum.out : treepathsum.c tree.h
	gcc -o $@ treepathsum.c 

treebltraverse.out : treebltraverse.c tree.h 
	gcc -o $@ treebltraverse.c

treemaxlevel.out : treemaxlevel.c tree.h
	gcc -o $@ treemaxlevel.c

treesymmetriccheck.out : treesymmetriccheck.c tree.h
	gcc -o $@ treesymmetriccheck.c

treetraverse.out : treetraverse.c treetraverse-main.c tree.h treetraverse.h
	gcc -o $@ treetraverse.c treetraverse-main.c

treebuild.out : treebuild.c treetraverse.c tree.h treetraverse.h
	gcc -o $@ treebuild.c treetraverse.c

treelca.out : treelca.c treetraverse.c tree.h treetraverse.h
	gcc -o $@ treelca.c treetraverse.c

clean:
	rm -f *.out *.o

