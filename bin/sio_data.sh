#!/bin/bash -

set -euo pipefail
set -x

self="$0"
sio="$1"

self=$(realpath "$self")
here=$(dirname "$self")
top=$(dirname "$here")

cd "$top"

[ -p $sio.rx ] || mkfifo $sio.rx
[ -p $sio.tx ] || mkfifo $sio.tx

stty raw

exec 8<"$sio.rx"
exec 9<"$sio.tx"

while true
do
    printf '\r\nstarting write to %s\r\n' "$sio.tx"
    cat > "$sio.tx"
    sleep 1
done
