#!/bin/sh
#
# Copyright © 2023 Chee Bin HOH. All rights reserved.
#
# It combines 5 steps into one shell script:
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

for f in `git diff --name-only`; do 
   if [ ! -x $f ]; then
       trim-space.sh $f
   fi
done

echo "make clean"
make clean


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
