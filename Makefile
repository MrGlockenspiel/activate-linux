CC       = rustc
SOURCES  = $(wildcard *.rs)
SOURCES  = $(wildcard *.rs)
BINARY  = activate_linux_cli

<< := @echo
PKGS := cairo

.PHONY: all clean test

$(BINARY): 
	$(CC) -A dead_code main.rs -o $(BINARY) $(RFLAGS)

install: $(BINARY)
	mv $(BINARY) /usr/local/bin/$(BINARY) 

uninstall:
	$(RM) /usr/local/bin/$(BINARY)

clean:
	$(RM) $(BINARY)
# build and run
test: test $(TARGETS)
	./(BINARY)
