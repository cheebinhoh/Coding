#!/bin/sh
#
# Copyright © 2022 Chee Bin HOH. All rights reserved.
#
# A front end to cntdown
#
# Happy coding!  .... note that I am always a C programmer before I am anything else in computing,
# so I always declare variable in the proper scope even bourne shell has a loose scope definition
# and variable just spawn into existence in outer scope if it is assigned value in inner scope,


displayHelp()
{
    echo "Usage: `basename $0` [-H hours -M minutes -S seconds -b -s -m \"message at end\" -f \"%Y-%m-%d %H:%M:%S\"] \"2022-01-21 12:00:00\""
}


# parameters parsing

silent=""
beep=""
adjustHour=""
adjustMin=""
adjustSecond=""
while getopts "H:M:S:bm:sf:h" opt; do
    case "$opt" in
        H)
            adjustHour="${OPTARG}"
            ;;
        M)  
            adjustMin="${OPTARG}"
            ;;
        S)
            adjustSecond="${OPTARG}"
            ;;
        b)
            beep="yes"
            ;;
        s)
            silent="yes"
            ;;

        f)
            formatOption="-f ${OPTARG}"
            ;;

        m)
            msg=${OPTARG}
            ;;

        *)
            displayHelp
            exit 1
    esac
done

adjustDateTimeParams=""
if [ "$adjustHour" != "" ]; then

    adjustDateTimeParams="-v +${adjustHour}H"
fi

if [ "$adjustMin" != "" ]; then

    adjustDateTimeParams="$adjustDateTimeParams -v +${adjustMin}M"
fi

if [ "$adjustSecond" != "" ]; then

    adjustDateTimeParams="$adjustDateTimeParams -v +${adjustSecond}S"
fi

dateTimeVal=""
if [ "$adjustDateTimeParams" != ""  ]; then

    dateTimeVal=`date $adjustDateTimeParams "+%Y-%m-%d %H:%M:%S"`
fi

shift $((OPTIND-1))

if [ "$#" -eq "1" ]; then
 
    dateTimeVal="$*"
fi

if [ "$dateTimeVal" == "" ]; then
 
    displayHelp
    exit 1
fi

# main body

remain=`./cntdown.out ${formatOption+"$formatOption"} "$dateTimeVal"`
runResult=$?

if [ "$runResult" -ne "0" ]; then

    exit $runResult
fi

while [ "${remain}" -gt "0" ]; do

    if [ "${silent}" != "yes" ]; then

        echo "${remain}"
    fi

    sleep 1;

    remain=`./cntdown.out ${formatOption+"$formatOption"} "$dateTimeVal"`
done

if [ "${remain}" -eq "0" ]; then

    if [ "${silent}" != "yes" ]; then

        echo ${msg:-0}
    fi

    if [ "${beep}" == "yes" ]; then

        echo '\007\c'
    fi
fi
