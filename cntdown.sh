#!/bin/sh
#
# Copyright © 2022 Chee Bin HOH. All rights reserved.
#
# A front end to cntdown
#
# Happy coding!

args=`getopt sf: $*`
# you should not use `getopt abo: "$@"` since that would parse
# the arguments differently from what the set command below does.
if [ $? != 0 ]; then

    echo "Usage: `basename $0` -s [-f \"%Y-%m-%d %H:%M:%S\"] \"2022-01-21 12:00:00\""
    echo
    echo "    -s : silent"
    exit 2
fi

silent=""
set -- $args

for i; do
    case "$i" in
        -s) silent="yes"
            shift;;

        --) shift
            break;;
    esac
done

remain=`./cntdown.out "$*"`
runResult=$?

if [ "$runResult" -ne "0" ]; then

    exit $runResult
fi

while [ "${remain}" -gt "0" ]; do

    if [ "${silent}" != "yes" ]; then

        echo "${remain}"
    fi

    sleep 1;
    remain=`./cntdown.out "$*"`
done

if [ "${silent}" != "yes" ]; then

    if [ "${remain}" -eq "0" ]; then

        echo 0
    fi
fi
