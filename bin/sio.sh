#!/bin/bash -

self="$0"

self=$(realpath "$self")
here=$(dirname "$self")
top=$(dirname "$here")

cd "$top"

set -euo pipefail
set -x

xterm \
    -geometry 64x16 \
    -e bin/sio_data.sh dev/sys.sio
