#!/bin/sh
#
# Copyright © 2023 Chee Bin HOH. All rights reserved.
#
# It combines the following steps into one shell script:
# - remove padding space and detect files with tab at start of lines
# - make all to catch build error
# - make clean to remove binary object and executable files
# - clang-format files to be committed (in directory and file extension
#   configured via DIRS file)
# - git add changes
# - git commit changes
# - git push changes

oldpwd=$PWD
rootdir=`dirname $0`
if [ $rootdir != "" ]; then
  cd $rootdir
fi

make trim-space.out >/dev/null
if [ ! -x trim-space.out ]; then
  echo "trim-space.out fails to be built"
  exit 1
fi

function source_files
{
  IFS_PREV=$IFS
  IFS=$'\n'

  for l in `cat DIRS`; do
    IFS=$IFS_PREV

    dir=`echo $l | sed -e 's/\(.*\):.*/\1/g'`
    exts=`echo $l | sed -e 's/.*://g'`

    cd $dir

    for file in $exts; do
      echo $dir/$file
    done

    cd - >/dev/null
  done

  IFS=$IFS_PREV
}

# extra space following newline is never intended to be checked in, so we trim it.
#
# Tab at the beginning of lines are not consistent cross IDE, it is particular
# annoying for source files saved in visual studio kind of IDE and reopen in
# vi.

echo "Trim space and check tab at the start of lines..."

has_invalid_tab=""
for f in `source_files`; do
  if [ ! -x $f ]; then
    ./trim-space.sh $f
  fi

  invalid_tab_lines=`sed -n -e '/^\t/=' $f`
  if [ "$invalid_tab_lines" != "" ] ; then
    has_invalid_tab="yes"
    echo Error: $f has invalid tab at beginning of the lines: `echo $invalid_tab_lines | sed -e 's/ /, /g'`
  fi
done

if [ "$has_invalid_tab" == "yes" ]; then
  exit 1
fi

# test build, we do not want to check in things that break
echo "build check..."
if [ -f CMakeLists.txt ] ; then
  cmake .
fi

make >/dev/null || exit 1

# clean up build, we do not want to check in binary
echo "clean up build..."
make clean >/dev/null

# clang-format the source files
if which clang-format &>/dev/null; then
  echo "perform clang-format..."

  for f in `source_files`; do
    clang-format ${f} > ${f}_tmp
    if ! diff $f ${f}_tmp &>/dev/null; then
      cp ${f}_tmp ${f}
    fi

    rm ${f}_tmp
  done
fi

# git activities
echo "git add, commit and push..."
echo

git add .
git commit -m "${1:-"no comment"}"
git push origin `git branch  | grep '^*'  | sed -e 's/\*//g'` --force


if [ $rootdir != "" ]; then
  cd $oldpwd;
fi
