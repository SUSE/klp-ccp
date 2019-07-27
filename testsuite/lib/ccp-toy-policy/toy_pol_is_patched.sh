#!/bin/sh

fun="$1"

if echo "$fun" | grep -q 'p[use]_'; then
    echo "RESULT: YES"
else
    echo "RESULT: NO"
fi
