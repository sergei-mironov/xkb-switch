#!/bin/sh

LIB=`dirname $0`/build/libxkbswitch.so

if ! test -f "$LIB" ; then
        echo "$LIB not found. Try building it first" >&2
        exit 1
fi

if ! which gcc ; then
        echo "GCC not found" >&2
        exit 1
fi

STDLIB=$(gcc --print-file-name=libstdc++.so)
export LD_LIBRARY_PATH="`dirname $STDLIB`:$LD_LIBRARY_PATH"

cat >/tmp/vimxkbswitch <<EOF
let g:XkbSwitchLib = "$LIB"
echo libcall(g:XkbSwitchLib, 'Xkb_Switch_getXkbLayout', '')
call libcall(g:XkbSwitchLib, 'Xkb_Switch_setXkbLayout', 'us')
quit
EOF

vim -S /tmp/vimxkbswitch && echo OK

