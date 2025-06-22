#!/bin/sh
#
# Copyright © 2023 Chee Bin HOH. All rights reserved.
#

DIRS=". kafka test"
FILE_PATTERN='*.cpp *.hpp'

# extra space following newline is never intended to be checked in, so we trim it.
#
# Tab at the beginning of lines are not consistent cross IDE, it is particular
# annoying for source files saved in visual studio kind of IDE and reopen in
# vi.

echo "******** check for tab character..."

has_invalid_tab=""
for d in `echo ${DIRS}`; do
  cd $d;

  for f in `ls ${FILE_PATTERN} 2>/dev/null`; do
    invalid_tab_lines=`sed -n -e '/^\t/=' $f`
    if [ "$invalid_tab_lines" != "" ] ; then
      has_invalid_tab="yes"
      echo Error: $f has invalid tab at beginning of the lines: `echo $invalid_tab_lines | sed -e 's/ /, /g'`
    fi
  done

  cd - &>/dev/null
done

if [ "$has_invalid_tab" == "yes" ]; then
  exit 1
fi

# clang-format the source files
if which clang-format &>/dev/null; then
  echo "******** perform clang-format..."

  for d in `echo ${DIRS}`; do
    cd ${d};

    for f in `ls ${FILE_PATTERN} 2>/dev/null`; do
      clang-format ${f} > ${f}_tmp
      if ! diff $f ${f}_tmp &>/dev/null; then
        echo "- formatting ${f}..."
        cp ${f}_tmp ${f}
      fi

      rm ${f}_tmp
    done

    cd - &>/dev/null
  done
fi


