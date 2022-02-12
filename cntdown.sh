#!/bin/sh
#
# Copyright © 2022 Chee Bin HOH. All rights reserved.
#
# A front end to cntdown
#
# Happy coding!  .... note that I am always a C programmer before 
# I am anything else in computing, so the habit of declaring variable
# in the proper scope even bourne shell has a loose scope definition 
# and variable just spawn into existence in outer scope if it is 
# assigned value in inner scope.


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

# FIXME: put up an autoconf system to detect version of date and then customize the file, 
# the other way is to use more platform independent language, like python, but who need 
# python when you have bourne shell.
#
# for some reason if we try to run "date" as a command inside the "if", like "if date -v +1H 2>dev/null; then ..."
# two instances of the command is spawned in ubuntu shell for WSL that I test, so I change it to capture 
# output.

dateResult=$(date -v +1H 2>/dev/null)

if [ "$dateResult" = "" ]; then
    linuxDate="yes"
else
    linuxDate=""
fi

adjustDateTimeParams=""
if [ "$adjustHour" != "" ]; then

    if [ "$linuxDate" = "yes" ]; then
       while [ "$adjustHour" -gt "0" ]; do
           adjustDateTimeParams="${adjustDateTimeParams} next hour"
           adjustHour=$((adjustHour -  1))
       done
    else
       adjustDateTimeParams="-v +${adjustHour}H"
    fi
fi

if [ "$adjustMin" != "" ]; then

    adjustDateTimeParams="$adjustDateTimeParams -v +${adjustMin}M"
fi

if [ "$adjustSecond" != "" ]; then

    adjustDateTimeParams="$adjustDateTimeParams -v +${adjustSecond}S"
fi

dateTimeVal=""
if [ "$adjustDateTimeParams" != ""  ]; then

    if [ "$linuxDate" = "yes" ]; then
       dateTimeVal=`date --date="$adjustDateTimeParams" +"%Y-%m-%d %H:%M:%S"`
    else
       dateTimeVal=`date $adjustDateTimeParams "+%Y-%m-%d %H:%M:%S"`
    fi
fi

shift $((OPTIND-1))

if [ "$#" -eq "1" ]; then
 
    dateTimeVal="$*"
fi

if [ "$dateTimeVal" = "" ]; then
 
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

        echo "\r                        \c" # a quick hack :)
        echo "\r${remain}\c";
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
