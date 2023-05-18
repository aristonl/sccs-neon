E=@echo
Q=@

PREFIX=$(HOME)

all:
	$(Q) $(MAKE) -C src

clean:
	$(Q) $(MAKE) -C src clean
	$(Q) rm -rf bin/*

install: all
	$(Q) install bin/neon '$(PREFIX)/bin'
	$(E) "  ==> INSTALL		neon"
	$(Q) $(MAKE) PREFIX=$(PREFIX) -C src install
