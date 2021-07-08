#!/bin/sh

make >/dev/null || (echo "Error in generating trim-spave";  exit 1)

for f 
do
    cat $f | ./trim-space > ${f}.$$ && mv ${f}.$$ $f
done
