#!/bin/sh
#
# $Id: lsd.sh,v 1.1 2008/12/05 16:07:42 urs Exp $

for arg in "$@"; do (
    if [ -d "$arg" ]; then
	cd "$arg"
	find -print0 | sort -z | xargs -0 lssum | md5sum |
		sed "s|-|$arg|"
    else
	echo .
    fi
); done
