#!/bin/bash -

self="$0"

self=$(realpath "$self")
here=$(dirname "$self")

while true
do
    $here/build-wait.sh
done
