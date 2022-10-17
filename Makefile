CC       = rustc
SOURCES  = $(wildcard *.rs)
BINARY  = activate-linux

<< := @echo
PKGS := cairo

.PHONY: all clean test

activate-linux: 
	$(CC) -A dead_code main.rs -o $(BINARY) $(RFLAGS)

install: $(BINARY)
	install -Dm0755 $(BINARY) $(DESTDIR)$(PREFIX)/$(BINDIR)/$(BINARY)

uninstall:
	$(RM) /usr/local/bin/$(BINARY)

clean:
	$(RM) $(BINARY)
# build and run
test: test $(TARGETS)
	./(BINARY)
