#!/bin/sh

fun="$1"
retries="$2"

fun="klpr_$fun"
if [ "$retries" != "0" ]; then
    fun="${fun}${retries}"
fi

echo "RESULT: $fun"
