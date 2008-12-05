#!/bin/sh
#
# $Id: lsdiff.sh,v 1.1 2008/12/05 16:07:42 urs Exp $

test -d "$1" -a -d "$2" || { echo error ; exit 1 ; }

(cd "$1"; find -print0 | sort -z | xargs -0 lssum >/tmp/lss.1)
(cd "$2"; find -print0 | sort -z | xargs -0 lssum >/tmp/lss.2)

diff -u /tmp/lss.1 /tmp/lss.2
