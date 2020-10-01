set -e

make debug

XEPHYR=$(whereis -b Xephyr | cut -f2 -d' ')
xinit ./xinitrc -- \
	"$XEPHYR" \
	:100 \
	-ac \
	-screen 1200x700 \
	-host-cursor
