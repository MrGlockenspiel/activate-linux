CC ?= clang
CFLAGS ?= -Og -Wall -Wpedantic -Wextra
PREFIX ?= /usr/local
BINDIR ?= bin
DESTDIR ?=

<< := @echo

ifneq ($(shell eval 'echo -e'),-e)
	<< += -e
endif

PKGS := \
	x11 xfixes xinerama xrandr \
	wayland-client cairo

RM := rm

WAYLAND_PROTOCOLS_DIR := $(shell pkg-config --variable=pkgdatadir wayland-protocols)
WAYLAND_PROTOCOL_HEADERS := protocols

SOURCES := $(wildcard src/*.c)
OBJECTS := $(SOURCES:src/%.c=obj/%.o)

GENERATORS := $(wildcard src/*.cgen)
OBJECTS += $(GENERATORS:src/%.cgen=obj/%.o)

NAME := $(shell uname -s)
CFLAGS := \
	$(CFLAGS) \
	-I$(WAYLAND_PROTOCOL_HEADERS) \
	$(shell pkg-config --cflags $(PKGS))

LDFLAGS := \
	$(LDFLAGS) \
	$(shell pkg-config --libs $(PKGS))

ifeq ($(NAME),Linux)
	BINARY := activate-linux
endif

ifeq ($(NAME),Darwin)
	BINARY := activate-macos
endif

all: $(BINARY)

obj/%.o: src/%.c
  mkdir -p $(WAYLAND_PROTOCOL_HEADERS)
	wayland-scanner private-code $(WAYLAND_PROTOCOLS_DIR)/stable/xdg-shell/xdg-shell.xml $(WAYLAND_PROTOCOL_HEADERS)/xdg-shell.c
	wayland-scanner client-header wlr-layer-shell-unstable-v1.xml $(WAYLAND_PROTOCOL_HEADERS)/wlr-layer-shell-unstable-v1.h
	wayland-scanner private-code wlr-layer-shell-unstable-v1.xml $(WAYLAND_PROTOCOL_HEADERS)/wlr-layer-shell-unstable-v1.c
	$(<<) "  CC\t" $(<)
	@$(CC) -c $(<) $(WAYLAND_PROTOCOL_HEADERS)/*.c -o $(@) $(CFLAGS)

%.c: %.cgen
	$(<<) " GEN\t" $(<)
	@sh $(<) > $(@)

$(BINARY): $(OBJECTS)
	$(<<) "LINK\t" $(^)
	@$(CC) $(^) -o $(@) $(LDFLAGS)

install: $(BINARY)
	install -Dm0755 $(BINARY) $(DESTDIR)$(PREFIX)/$(BINDIR)/$(BINARY)

uninstall:
	$(RM) -f $(DESTDIR)$(PREFIX)/$(BINDIR)/$(BINARY)

clean:
	$(RM) -f $(OBJECTS) $(BINARY)

test: $(BINARY)
	./$(BINARY)

.PHONY: all clean install test
