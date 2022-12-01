#!/bin/sh
# 
# it combines 3 commands into one

if which clang-format &>/dev/null; then
  for cf in *.c *.h; do
    clang-format ${cf} > ${cf}_tmp 
    if ! diff $cf ${cf}_tmp &>/dev/null; then
      cp ${cf}_tmp ${cf}
    fi

    rm ${cf}_tmp
  done
fi

git add .
git commit -m "${1:-"no comment"}"
git push origin master --force
