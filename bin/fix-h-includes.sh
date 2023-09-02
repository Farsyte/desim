#!/bin/bash -
# set -euo pipefail
# set -x

for h in "$@"
do
    d=$(dirname "$h")
    b=$(basename "$h")
    o=$d/$b.fix-h-includes

    (
        echo '#pragma once' > $o
        echo
        grep '#include' < $h | sort
        echo
        grep -v '#pragma once' < $h | grep -v '#include'
    ) | cat -s >> $o

    if cmp $h $o >/dev/null
    then
        rm -f "$o"
    else
        mv "$h" "$h"~
        mv "$o" "$h"
    fi
done
