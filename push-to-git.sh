#!/bin/sh

git add .
git commit -m "${1:-"no comment"}"
git push origin master
