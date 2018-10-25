#!/bin/sh
#
# $Id: lsdiff.sh,v 1.3 2018/10/25 05:33:31 urs Exp $

test -d "$1" -a -d "$2" || { echo error ; exit 1 ; }

(cd "$1"; find -print0 | sort -z | xargs -0 lssum >/tmp/lss.1)
(cd "$2"; find -print0 | sort -z | xargs -0 lssum >/tmp/lss.2)

diff -u /tmp/lss.1 /tmp/lss.2
