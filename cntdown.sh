#!/bin/sh
#
# Copyright © 2022 Chee Bin HOH. All rights reserved.
#
# A front end to cntdown
#
# Happy coding!

silent=""
beep=""
while getopts "bm:sf:" opt; do
    case "$opt" in
        b)
            beep="yes"
            ;;
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


if [ "${remain}" -eq "0" ]; then

    if [ "${silent}" != "yes" ]; then

        echo ${msg:-0}
    fi

    if [ "${beep}" == "yes" ]; then

        echo '\007\c'
    fi
fi
