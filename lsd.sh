#!/bin/sh
#
# $Id: lsd.sh,v 1.3 2018/10/25 05:33:31 urs Exp $

for arg in "$@"; do (
    if [ -d "$arg" ]; then
	cd "$arg"
	find -print0 | sort -z | xargs -0 lssum | md5sum | sed "s|-|$arg|"
    else
	echo .
    fi
); done
