#!/bin/bash -
clear
date
echo
make loop
make showbuildlogs
make showstderr
inotifywait -e modify -e delete inc/*.h src/*.c bin/*
exec "$0" "$@"
