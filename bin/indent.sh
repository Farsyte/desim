#!/bin/bash -
set -euo pipefail
# set -x

rp=$(realpath "$0")
dp=$(dirname "$rp")
    
for f in "$@"
do
    o="$f".indented
    indent "$f" -o "$o"
    if cmp "$f" "$o" > /dev/null
    then
        rm -f "$o"
    else
        mv "$f" "$f"~
        mv "$o" "$f"
    fi
done
