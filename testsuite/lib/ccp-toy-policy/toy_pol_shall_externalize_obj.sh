#!/bin/sh

obj="$1"

espec="$(echo "$obj" | sed 's/^\(e[use]\(p\)\?\(r\)\?_\)\?.*/\1/')"

if [ -n "$espec" ]; then
    echo "RESULT: YES"
else
    echo "RESULT: NO"
fi
