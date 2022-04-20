CC       = clang
SOURCES  = $(wildcard *.c)
TARGETS  = activate-linux

RM = rm
name := $(shell uname -s)

# Linux specific flags
ifeq ($(name),Linux)
    BINARY  = activate-linux
	CFLAGS  = -lX11 -lXfixes -lXinerama -lcairo -I/usr/include/cairo
endif

# OSX specific flags
ifeq ($(name),Darwin)
    BINARY  = activate-macos
	CFLAGS  = -lX11 -lXfixes -lXinerama -lcairo -I/opt/local/include/cairo -I/opt/X11/include
endif

.PHONY: all clean test

all: $(TARGETS)

activate-linux: 
	rm -f -r bin
	mkdir bin
	$(CC) src/activate_linux.c -o bin/$(BINARY) $(CFLAGS)

# install to /usr/local/bin
# the chmod is needed because the Makefile is run as root and clean wont work as a user without it
install: $(TARGETS)
	chmod 777 bin/
	cp bin/$(BINARY) /usr/local/bin/

# uninstall binary
uninstall:
	rm /usr/local/bin/$(BINARY)

# clean
clean:
	$(RM) -rf bin/
	
# build and run
test: test $(TARGETS)
	./bin/$(BINARY)