#!/bin/sh
# 
# it combines 3 commands into one

for cf in *.c; do
  echo $cf
done

git add .
git commit -m "${1:-"no comment"}"
git push origin master --force
