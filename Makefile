CC       = clang
SOURCES  = $(wildcard *.c)
TARGETS  = activate_linux

RM = rm
name := $(shell uname -s)

# Linux specific flags
ifeq ($(name),Linux)
    BINARY  = activate_linux
	CFLAGS  = -lX11 -lXfixes -lcairo -I/usr/include/cairo
endif

# OSX specific flags
ifeq ($(name),Darwin)
    BINARY  = activate_macos
	CFLAGS  = -lX11 -lXfixes -lcairo -I/usr/local/Cellar/cairo/1.16.0_5/include/cairo -I/opt/X11/include
endif

.PHONY: all clean test

all: $(TARGETS)

activate_linux: 
	rm -f -r bin
	mkdir bin
	$(CC) src/activate_linux.c -o bin/$(BINARY) $(CFLAGS)

# clean
clean:
	$(RM) bin/$(BINARY)
	
# build and run
test: test $(TARGETS)
	./bin/$(BINARY)