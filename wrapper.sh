#!/bin/sh
#
# Copyright © 2023 Chee Bin HOH. All rights reserved.

PATH=$PATH:.

dirName=`dirname $0`
programName=${dirName}/`basename $0 | sed -e 's/\...$//g'`.out

if [[ ! -x ${programName} ]] ; then

    cd ${dirName}

    make `basename ${programName}` >/dev/null || (echo "Error in running ${pogramName}";  exit 1)

    cd -
fi

trap "rm *.$$" INT

for f
do
    cat $f | ${programName} > ${f}.$$ && mv ${f}.$$ $f
done
