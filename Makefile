CC       = rustc
SOURCES  = $(wildcard *.rs)
TARGETS  = activate-linux

<< := @echo
PKGS := cairo

.PHONY: all clean test

activate-linux: 
	$(CC) main.rs -o $(BINARY) $(RFLAGS)

install: $(BINARY)
	install -Dm0755 $(BINARY) $(DESTDIR)$(PREFIX)/$(BINDIR)/$(BINARY)

uninstall:
	$(RM) /usr/local/bin/$(BINARY)

clean:
	$(RM) $(BINARY)
# build and run
test: test $(TARGETS)
	./(BINARY)
