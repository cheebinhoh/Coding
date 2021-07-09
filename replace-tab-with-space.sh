#!/bin/sh

make >/dev/null || (echo "Error in generating trim-spave";  exit 1)

for f 
do
    cat $f | ./replace-tab-with-space.out > ${f}.$$ && mv ${f}.$$ $f
done
