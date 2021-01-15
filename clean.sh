#!/bin/sh
set -e
. ./config.sh

for PROJECT in $PROJECTS; do
  printf "\n[ \033[1;96mCLEANING: \033[1;95m%s\033[0m ]\n\n" $PROJECT
  (cd $PROJECT && $MAKE clean)
done

rm -rf sysroot
rm -rf build