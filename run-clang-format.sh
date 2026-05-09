#!/bin/sh
#
# Copyright © 2023 Chee Bin HOH. All rights reserved.
#

DIRS=". gnome/glib gnome/gio gnome/gtk"
FILE_PATTERN='*.c *.h *.cpp *.hpp'

# extra space following newline is never intended to be checked in, so we trim it.
#
# Tab at the beginning of lines are not consistent cross IDE, it is particular
# annoying for source files saved in visual studio kind of IDE and reopen in
# vi.

ROOT_DIR=`pwd`

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

  cd $ROOT_DIR
done

if [ "$has_invalid_tab" = "yes" ]; then
  exit 1
fi

# clang-format the source files
if which clang-format &>/dev/null; then
  echo "******** perform clang-format..."

  for d in `echo ${DIRS}`; do
    cd ${d};

    for f in `ls ${FILE_PATTERN} 2>/dev/null`; do
      clang-format -i --style=LLVM ${f}
    done

    cd $ROOT_DIR
  done
fi
