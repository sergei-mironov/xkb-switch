#!/bin/sh

die() { echo $@ >&2 ; exit 1 ; }
# debug() { echo $@ >&2 ; }
debug() { return 0 ;}

usage() { (
	printf "`basename $0` waits for any user input, then switches between primary and secondary layout\n"
	printf "Usage: `basename $0` LAY1 LAY2\n"
	printf "Where\n"
	printf "\tLAY1 - primary layout\n"
	printf "\tLAY2 - initial secondary layout\n"
	printf "Example:\n"
	printf "`basename $0` us ru - switches between 'us' and some secondary layout, initially 'ru'\n"
)>&2 ; }

while test -n "$1" ; do
	case $1 in
		-h|--help)
			usage ; die ;;
		*)
			lay1=$1
			lay2=$2
			shift
			;;
	esac
	shift
done

test -n "$lay1" || { die "error: LAY1 is empty, try --help"; }
test -n "$lay2" || { die "error: LAY2 is empty, try --help"; }

FIFO=/tmp/$USER-kde-groups-emu.fifo
rm $FIFO >/dev/null 2>&1
mkfifo $FIFO 2>/dev/null || die "Failed to create $FIFO"

while read event arg <$FIFO ; do
	debug "$event $arg" >&2
	case $event in
		layout-change)
			lay=$arg
			case $lay in
				$lay1) next=$lay2;;
				*) next=$lay1
				   lay2=$lay;;
			esac
			;;
		user-asks-switch)
			xkb-switch -s $next ;;
		*)
			echo "unknown command: $event $arg" >&2
			;;
	esac
done &

echo layout-change `xkb-switch -p` >$FIFO
xkb-switch -W | while read l ;do
	echo "layout-change $l" >$FIFO
done &

trap "pkill -P $$; exit" SIGINT
trap "pkill -P $$; exit" SIGTERM
trap "pkill -P $$; exit" EXIT

while read l ; do
	echo "user-asks-switch $l" >$FIFO
done

