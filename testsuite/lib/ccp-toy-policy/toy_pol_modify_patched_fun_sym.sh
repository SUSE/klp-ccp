#!/bin/sh

fun="$1"
retries="$2"

pspec="$(echo "$fun" | sed 's/^\(p[use]\)\?_.*/\1/')"

if [ -z "$pspec" ]; then
    echo "error: logic error: $what \"$fun\" should be patched" 1>&2
    exit 1
fi


fun="klpp_$fun"
if [ "$retries" != "0" ]; then
    fun="${fun}${retries}"
fi

case "$(echo "$pspec" | cut -c 2)" in
    u)
	LC=KEEP-LINKAGE
	;;
    s)
	LC=MAKE-INTERNAL
	;;
    e)
	LC=MAKE-EXTERNAL
	;;
esac

echo "RESULT: $fun $LC"
