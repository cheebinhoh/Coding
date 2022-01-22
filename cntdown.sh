#!/bin/sh
#
# Copyright © 2022 Chee Bin HOH. All rights reserved.
#
# A front end to cntdown
#
# Happy coding!

silent=""
while getopts "m:sf:" opt; do
    case "$opt" in
        s)
            silent="yes"
            ;;

        f)
            formatOption="-f ${OPTARG} "
            ;;

        m)
            msg=${OPTARG}
            ;;

        *)
            echo "Usage: `basename $0` [-s -m \"message at end\" -f \"%Y-%m-%d %H:%M:%S\"] \"2022-01-21 12:00:00\""
            exit 1
    esac
done

shift $((OPTIND-1))

remain=`./cntdown.out ${formatOption+"$formatOption"} "$*"`
runResult=$?

if [ "$runResult" -ne "0" ]; then

    exit $runResult
fi

while [ "${remain}" -gt "0" ]; do

    if [ "${silent}" != "yes" ]; then

        echo "${remain}"
    fi

    sleep 1;

    remain=`./cntdown.out ${formatOption+"$formatOption"} "$*"`
done

if [ "${silent}" != "yes" ]; then

    if [ "${remain}" -eq "0" ]; then

        echo ${msg:-0}
    fi
fi
