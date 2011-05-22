#
# $Id: Makefile,v 1.1 2011/05/22 06:14:59 urs Exp $
#

INSTALL = /usr/bin/install
RM      = rm -f

CFLAGS  = -Os -Wall -Wextra
LDFLAGS = -s
LDLIBS  = -lcrypto
prefix  = /usr/local

.PHONY: all
all: lssum

.PHONY: install
install: lssum
	$(INSTALL) -m 755 lssum   $(prefix)/bin
	$(INSTALL) -m 644 lssum.1 $(prefix)/man/man1

.PHONY: clean
clean:
	$(RM) *.o core lssum
