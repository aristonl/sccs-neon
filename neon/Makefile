#   $Id: Makefile,v 1.1.1.1 2024/05/30 13:38:04 atl Exp $

PROG=neon neon-init-db neon-write-cache neon-read-cache
NEON_VERSION=v0.1-devel

CC= gcc
CFLAGS= --std=c89 -O2 -g -pedantic -DNEON_VERSION='"$(NEON_VERSION)"'

E=@echo
Q=@

all: $(PROG)

neon: neon.o
	$(E) "  ==> CCLD		$@"
	$(Q) $(CC) -o $@ neon.o $(CFLAGS)

neon-init-db: init-db.o
	$(E) "  ==> CCLD		$@"
	$(Q) $(CC) -o $@ init-db.o $(CFLAGS)

neon-write-cache: write_cache.o sha1.o
	$(E) "  ==> CCLD		$@"
	$(Q) $(CC) -o $@ write_cache.o sha1.o $(CFLAGS)

neon-read-cache: read_cache.o
	$(E) "  ==> CCLD		$@"
	$(Q) $(CC) -o $@ read_cache.o $(CFLAGS)

install: neon
	$(Q) cp $(PROG) ~/bin/
	

shatest: sha1.o shatest.o
	$(E) "  ==> CCLD		$@"
	$(Q) $(CC) -o $@ sha1.o shatest.o $(CFLAGS)

clean:
	$(E) "  ==> CLEAN"
	$(Q) rm -rf $(PROG) shatest *.o

.c.o:
	$(E) "  ==> CC		$@"
	$(Q) $(CC) $(CFLAGS) -c $*.c

neon.o: neon.c
write_cache.o: write_cache.c cache.h sha1.h
read_cache.o: read_cache.c cache.h
init-db.o: init-db.c
sha1.o: sha1.c sha1.h
shatest.o: shatest.c sha1.h
