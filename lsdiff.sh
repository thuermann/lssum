#!/bin/sh
#
# $Id: lsdiff.sh,v 1.2 2009/01/09 13:18:40 urs Exp $

test -d "$1" -a -d "$2" || { echo error ; exit 1 ; }

types="-type d -o -type f -o -type l"

(cd "$1"; find \( $types \) -print0 | sort -z | xargs -0 lssum >/tmp/lss.1)
(cd "$2"; find \( $types \) -print0 | sort -z | xargs -0 lssum >/tmp/lss.2)

diff -u /tmp/lss.1 /tmp/lss.2
