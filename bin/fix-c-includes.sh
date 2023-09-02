#!/bin/bash -
# set -euo pipefail
# set -x

for c in "$@"
do
    d=$(dirname "$c")
    b=$(basename "$c" .c)
    o=$d/$b.c.fix-c-includes

    (
        (
            printf '#include "%s.h"\n' "$b"
            echo
            grep '#include <' < $c | sort
            echo
            grep '#include "' < $c | sort
            ) | uniq.awk
        echo
        grep -v '#include' < $c
    ) | cat -s > $o

    if cmp $c $o >/dev/null
    then
        rm -f "$o"
    else
        mv "$c" "$c"~
        mv "$o" "$c"
    fi
done
