#!/bin/sh

fun="$1"
from_header="$2"

if [ x"$from_header" = xfrom-header ]; then
    echo "RESULT: NO"
else
    echo "RESULT: YES"
fi
