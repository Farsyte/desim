#!/bin/bash -

self="$0"

self=$(realpath "$self")
here=$(dirname "$self")
top=$(dirname "$here")

cd "$top"

cat <<EOF

     === === === === === === === === === === === === === === ===
                            STARTING BUILD
                   $(date)
     === === === === === === === === === === === === === === ===

EOF


make -kj loop
make logs

cat <<EOF

     --- --- --- --- --- --- --- --- --- --- --- --- --- --- ---
                           BUILD COMPLETED
                   $(date)
     --- --- --- --- --- --- --- --- --- --- --- --- --- --- ---

             waiting for next update that needs a build.

EOF


exec inotifywait -q -r \
    -e modify -e delete \
    inc src bist bin log GNUmakefile*
