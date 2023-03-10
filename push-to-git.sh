#!/bin/sh
#
# Copyright © 2023 Chee Bin HOH. All rights reserved.
#
# it does 4 things:
# - it clang-format files in extension mentioned in DIRS file;
# - it git add changes
# - it git commit changes
# - it git push changes

oldpwd=$PWD
rootdir=`dirname $0`
if [ $rootdir != "" ]; then
  cd $rootdir
fi

echo "make clean"
make clean # so that we do not commit binary

if which clang-format &>/dev/null; then
  echo
  echo "clang-format source files"

  IFS_PREV=$IFS
  IFS=$'\n'

  for dir_line in `cat DIRS`; do
    dir=`echo $dir_line | sed -e 's/:.*//g'`
    f_glob=`echo $dir_line | sed -e 's/.*://g'`

    if [ -d $dir ]; then

      cd $dir

      for f in `eval "ls $f_glob 2>/dev/null"`; do
        clang-format ${f} > ${f}_tmp 
        if ! diff $f ${f}_tmp &>/dev/null; then
          cp ${f}_tmp ${f}
        fi

        rm ${f}_tmp
      done

      cd - >/dev/null

    else
      echo "$dir not exist" >&2
    fi 
    
  done

  IFS=$IFS_PREV
fi

echo
echo "git add, commit and push"
git add .
git commit -m "${1:-"no comment"}"
git push origin master --force

if [ $rootdir != "" ]; then
  cd $oldpwd;
fi
