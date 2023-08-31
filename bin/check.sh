#!/bin/bash -
set -euo pipefail
# set -x

exp="$1"
shift
obs="$1"
shift

if [ ! -e "$exp" ]
then
    printf 'accept this output as reference output\n'
    cp -pvf "$obs" "$exp"
fi

if cmp "$exp" "$obs"
then
    printf 'PASS: %s matches %s\n' "$exp" "$obs"
    exit 0
fi

printf 'PASS: %s differs from %s\n' "$exp" "$obs"
diff -wu "$exp" "$obs"
exit 1
