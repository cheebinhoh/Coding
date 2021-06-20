#!/bin/sh
# 
# it combines 3 commands into one

git add .
git commit -m "${1:-"no comment"}"
git push origin master
