#!/bin/sh

set -e -x

LIB=./libxkbswitch.so
X=./xkb-switch

if ! test -f "$LIB" ; then
  echo "$LIB not found. Did you run ./test.sh from the build directory?" >&2
  exit 1
fi

if ! test -f "$X" ; then
  echo "$X not found. Did you run ./test.sh from the build directory?" >&2
  exit 1
fi

if ! which gcc ; then
  echo "GCC not found, check your environment" >&2
  exit 1
fi

if ! which vim; then
  echo "VIM is required for this test, check your environment" >&2
  exit 1
fi

if test -z "$DISPLAY" ; then
  echo "This test requires X-server connection" >&2
  exit 1
fi

not() {(
  set +e
  $@
  if test "$?" = "0" ; then
    exit 1
  else
    exit 0
  fi
)}

if which git; then
  git status -vv
fi
setxkbmap -query
"$X" --version
"$X" --version 2>&1 | grep -q xkb-switch
"$X" --help 2>&1 | grep -q 'xkb-switch -s ARG'
test "$($X --help)" = "$($X -h)"
test "$($X --list)" = "$($X -l)"
for l in $($X --list) ; do
  "$X" -s "$l"    # Set the layout
  "$X" -ds "$l"   # Set the layout with debug info
  "$X" --set="$l" # Set the layout using long form
  test "$($X -p)" = "$l"  # Make sure the layout is set
done
"$X" -n
"$X" --next
not "$X" -s fooo  # Sets non-zero error code
$X --fancy        # Fancy name
test "$($X --fancy)" != "$($X -p)"

cat >/tmp/vimxkbswitch <<EOF
let g:XkbSwitchLib = "$LIB"
echo libcall(g:XkbSwitchLib, 'Xkb_Switch_getXkbLayout', '')
call libcall(g:XkbSwitchLib, 'Xkb_Switch_setXkbLayout', 'us')
quit
EOF

STDLIB=$(gcc --print-file-name=libstdc++.so)
LD_LIBRARY_PATH="`dirname $STDLIB`:$LD_LIBRARY_PATH" \
  vim -S /tmp/vimxkbswitch

echo OK

