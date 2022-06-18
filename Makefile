CC ?= clang
CFLAGS ?= -Og -Wall -Wpedantic -Wextra
PREFIX ?= /usr/local
BINDIR ?= bin
DESTDIR ?=

RM = rm

SOURCES := $(wildcard src/*.c)
NAME := $(shell uname -s)
CFLAGS := \
	$(CFLAGS) \
	$(shell pkg-config --cflags --libs x11 xfixes xinerama xrandr) \
	$(shell pkg-config --cflags --libs wayland-client) \
	$(shell pkg-config --cflags --libs cairo)

ifeq ($(NAME),Linux)
	BINARY := activate-linux
endif

ifeq ($(NAME),Darwin)
	BINARY := activate-macos
endif


all: $(BINARY)

$(BINARY): $(SOURCES)
	$(CC) $(^) -o $(@) $(CFLAGS)

install: $(BINARY)
	install -Dm0755 $(BINARY) $(DESTDIR)$(PREFIX)/$(BINDIR)/$(BINARY)

uninstall:
	$(RM) -f $(DESTDIR)$(PREFIX)/$(BINDIR)/$(BINARY)

clean:
	$(RM) -f $(BINARY)

test: $(BINARY)
	./$(BINARY)

.PHONY: all clean install test
