CC ?= clang
CFLAGS ?= -Og -Wall -Wpedantic -Wextra
PREFIX ?= /usr/local
BINDIR ?= bin
DESTDIR ?=

RM = rm

WAYLAND_PROTOCOLS_DIR := $(shell pkg-config --variable=pkgdatadir wayland-protocols)
WAYLAND_PROTOCOL_HEADERS := protocols

SOURCES := $(wildcard src/*.c)
NAME := $(shell uname -s)
CFLAGS := \
	$(CFLAGS) \
	$(shell pkg-config --cflags --libs x11 xfixes xinerama xrandr) \
	$(shell pkg-config --cflags --libs wayland-client) \
	$(shell pkg-config --cflags --libs cairo) \
	-I$(WAYLAND_PROTOCOL_HEADERS)

ifeq ($(NAME),Linux)
	BINARY := activate-linux
endif

ifeq ($(NAME),Darwin)
	BINARY := activate-macos
endif


all: $(BINARY)

$(BINARY): $(SOURCES)
	mkdir -p $(WAYLAND_PROTOCOL_HEADERS)
	wayland-scanner private-code $(WAYLAND_PROTOCOLS_DIR)/stable/xdg-shell/xdg-shell.xml $(WAYLAND_PROTOCOL_HEADERS)/xdg-shell.c
	wayland-scanner client-header wlr-layer-shell-unstable-v1.xml $(WAYLAND_PROTOCOL_HEADERS)/wlr-layer-shell-unstable-v1.h
	wayland-scanner private-code wlr-layer-shell-unstable-v1.xml $(WAYLAND_PROTOCOL_HEADERS)/wlr-layer-shell-unstable-v1.c

	$(CC) $(^) $(WAYLAND_PROTOCOL_HEADERS)/*.c -o $(@) $(CFLAGS)

install: $(BINARY)
	install -Dm0755 $(BINARY) $(DESTDIR)$(PREFIX)/$(BINDIR)/$(BINARY)

uninstall:
	$(RM) -f $(DESTDIR)$(PREFIX)/$(BINDIR)/$(BINARY)

clean:
	$(RM) -f $(BINARY)

test: $(BINARY)
	./$(BINARY)

.PHONY: all clean install test
