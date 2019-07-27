#!/bin/sh

what="$1"
name="$2"
retries="$3"

case "$what" in
    "function")
	espec="$(echo "$name" | sed 's/^\(p[use]_\)\?\(e[use]\(p\)\?\(r\)\?\)_.*/\2/')"
	;;

    "object")
	espec="$(echo "$name" | sed 's/^\(e[use]\(p\)\?\(r\)\?\)_.*/\1/')"
	;;
esac

if [ -z "$espec" ]; then
    echo "error: logic error: $what \"$name\" should be externalizable" 1>&2
    exit 1
fi


if echo "$espec" | grep -q 'r'; then
    name="klpe_$name"
    if [ "$retries" != "0" ]; then
	name="${name}${retries}"
    fi
fi

case "$(echo "$espec" | cut -c 2)" in
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

MP=
if echo "$espec" | grep -q 'p'; then
    MP=MAKE-POINTER
fi

echo "RESULT: $name $LC $MP"
