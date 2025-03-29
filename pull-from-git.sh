#!/bin/sh
#
# Copyright © 2023 Chee Bin HOH. All rights reserved.
#
# A shell script to pull and fetch from master

git fetch .
git pull origin `git branch  | grep '^*'  | sed -e 's/\*//g'`
