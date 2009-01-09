#!/bin/sh
#
# $Id: lsd.sh,v 1.2 2009/01/09 13:18:40 urs Exp $

types="-type d -o -type f -o -type l"

for arg in "$@"; do (
    if [ -d "$arg" ]; then
	cd "$arg"
	find \( $types \) -print0 | sort -z | xargs -0 lssum | md5sum |
		sed "s|-|$arg|"
    else
	echo .
    fi
); done
