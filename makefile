# all good makefile to help compiling

all : treepathsum.out treebltraverse.out treemaxlevel.out treesysmetriccheck.out treetraverse.out \
	treebuild.out treelca.out

treepathsum.out : treepathsum.c tree.h
	gcc -o $@ treepathsum.c 

treebltraverse.out : treebltraverse.c tree.h 
	gcc -o $@ treebltraverse.c

treemaxlevel.out : treemaxlevel.c tree.h
	gcc -o $@ treesysmetriccheck.c

treesysmetriccheck.out : treesysmetriccheck.c tree.h
	gcc -o $@ treesysmetriccheck.c

treetraverse.out : treetraverse.c treetraverse-main.c tree.h treetraverse.h
	gcc -o $@ treetraverse.c treetraverse-main.c

treebuild.out : treebuild.c treetraverse.c tree.h treetraverse.h
	gcc -o $@ treebuild.c treetraverse.c

treelca.out : treelca.c tree.h
	gcc -o $@ treelca.c

clean:
	rm -f *.out *.o
