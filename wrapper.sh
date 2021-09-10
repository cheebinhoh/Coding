#!/bin/sh


PATH=$PATH:.

programName=`basename $0 | sed -e 's/\...$//g'`.out

make ${programName} >/dev/null || (echo "Error in running ${pogramName}";  exit 1)

for f
do
    cat $f | ${programName} > ${f}.$$ && mv ${f}.$$ $f
done
