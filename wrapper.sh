#!/bin/sh

PATH=$PATH:.

programName=`basename $0 | sed -e 's/\...$//g'`

make ${programName}.out >/dev/null || (echo "Error in generating trim-spave";  exit 1)

for f
do
    cat $f | ${programName}.out > ${f}.$$ && mv ${f}.$$ $f
done
