#!/bin/sh
set -e
. scripts//config.sh

mkdir -p "$SYSROOT"

for PROJECT in $SYSTEM_HEADER_PROJECTS; do
  printf "\n[ \033[1;93mINSTALLING HEADERS: \033[1;95m%s\033[0m ]\n\n" $PROJECT
  (cd $PROJECT && DESTDIR="$SYSROOT" $MAKE install-headers)
done
