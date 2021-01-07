#!/bin/sh
set -e
. ./headers.sh

for PROJECT in $PROJECTS; do
  printf "\n[ \033[1;92mBUILDING: \033[1;95m%s\033[0m ]\n\n" $PROJECT
  (cd $PROJECT && DESTDIR="$SYSROOT" $MAKE install)
done
