#!/bin/sh

header="$(basename "$1")"
pre_include="$2"

if echo "$header" | grep -q 'internal'; then
    echo "RESULT: NO"
else
    echo "RESULT: YES"
fi
