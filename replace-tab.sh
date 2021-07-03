#!/bin/sh

for f;
do
    cat $f | sed -e 's/\t/    /g' > ${f}.$$ && mv ${f}.$$ ${f}
done
