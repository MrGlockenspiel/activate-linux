CC       = rustc
SOURCES  = $(wildcard *.rs)
TARGETS  = activate-linux

RM = rm
name := $(shell uname -s)

.PHONY: all clean test

all: $(TARGETS)

activate-linux: 
	$(CC) main.rs -o $(BINARY) $(RFLAGS)

# install to /usr/local/bin
# the chmod is needed because the Makefile is run as root and clean wont work as a user without it
install: $(TARGETS)
	chmod 777 bin/
	cp bin/$(BINARY) /usr/local/bin/

# uninstall binary
uninstall:
	$(RM) /usr/local/bin/$(BINARY)

# clean
clean:
	$(RM) -rf bin/
	
# build and run
test: test $(TARGETS)
	./(BINARY)
