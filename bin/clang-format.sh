#!/bin/bash -
set -euo pipefail
# set -x

rp=$(realpath "$0")
dp=$(dirname "$rp")
dp=$(realpath "$dp"/.clang)

for s in LLVM GNU Google Chromium Microsoft Mozilla WebKit
do
    fn="$dp"/"$s".clang-format
    test -e "$fn" ||
        clang-format --style=$s --dump-config > $fn
done
    
cfw="$dp"/WebKit.clang-format
cfg="$dp"/Farsyte.clang-format

test -e "$cfg" ||
    cp "$cfw" "$cfg"
    
for f in "$@"
do
    o="$f".indented
    clang-format \
        --style=file:"$cfg" \
        < "$f" > "$o"
    if cmp "$f" "$o" > /dev/null
    then
        rm -f "$o"
    else
        mv "$f" "$f"~
        mv "$o" "$f"
    fi
done
