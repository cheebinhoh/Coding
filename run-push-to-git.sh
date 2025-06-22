#!/bin/sh
#
# Copyright © 2023 Chee Bin HOH. All rights reserved.
#
# It combines the following steps into one shell script:
# - remove padding space and detect files with tab at start of lines
# - make all to catch build error
# - make clean to remove binary object and executable files
# - clang-format files to be committed (in directory and file extension
#   configured via DIRS_SOURCES file)
# - git add changes
# - git commit changes
# - git push changes

buildTest=""
commitMessage=""

while [ "$1" != "" ]; do
  case "$1" in
    -t)
      buildTest="yes"
      shift
      ;;

   -m)
     shift
     commitMessage="$1"
     shift
  esac
done

oldpwd=$PWD
rootdir=`dirname $0`
if [ $rootdir != "" ]; then
  cd $rootdir
fi

. ${rootdir}/run-clang-format.sh

# test build, we do not want to check in things that break
if [ "${buildTest}" == "yes" ]; then
  echo "******** run build..."
  if [ ! -d ./Build ]; then
    mkdir ./Build
  fi

  cd ./Build
  cmake ../

  make

  echo "******** run ctest..."
  ctest -L dmn

  # clean up build, we do not want to check in binary
  echo "******** clean up build..."
  make clean

  cd ../
fi

# git activities
echo "******** git add, commit and push..."
echo

git add .
git commit -m "${commitMessage:-"no comment"}"
git push origin `git branch  | grep '^*'  | sed -e 's/\*//g'` --force


if [ $rootdir != "" ]; then
  cd $oldpwd;
fi
