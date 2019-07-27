#!/bin/sh

fun="$1"
linkage="$2"
fundef_loc="$3"

espec="$(echo "$fun" | sed 's/^\(p[use]_\)\?\(e[use]\(p\)\?\(r\)\?_\)\?.*/\2/')"

if [ -n "$espec" ]; then
    echo "RESULT: YES"
else
    echo "RESULT: NO"
fi
