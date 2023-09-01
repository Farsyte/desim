#!/bin/bash -

self="$0"

self=$(realpath "$self")
here=$(dirname "$self")
top=$(dirname "$here")

cd "$top"

clear
date
echo

make loop
make logs

echo waiting for next update that needs a build.

exec inotifywait -q -r \
    -e modify -e delete \
    inc src bin log GNUmakefile*
