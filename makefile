# all good makefile to help compiling

all : treepathsum.out treebltraverse.out treemaxlevel.out treesysmetriccheck.out treetraverse.out \
	treebuild.out treelca.out

treepathsum.out : treepathsum.c
	gcc -o $@ $^

treebltraverse.out : treebltraverse.c
	gcc -o $@ $^

treemaxlevel.out : treemaxlevel.c
	gcc -o $@ $^

treesysmetriccheck.out : treesysmetriccheck.c
	gcc -o $@ $^

treetraverse.out : treetraverse.c
	gcc -o $@ $^

treebuild.out : treebuild.c
	gcc -o $@ $^

treelca.out : treelca.c
	gcc -o $@ $^

clean:
	rm -f *.out
