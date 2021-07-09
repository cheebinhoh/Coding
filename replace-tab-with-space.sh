#!/bin/sh
# Copyright © 2021 Chee Bin HOH. All rights reserved.


make >/dev/null || (echo "Error in generating trim-spave";  exit 1)


for f 
do
    cat $f | ./replace-tab-with-space.out > ${f}.$$ && mv ${f}.$$ $f
done
