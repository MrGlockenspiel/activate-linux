CC ?= clang
CFLAGS ?= -Og -Wall -Wpedantic -Wextra
PREFIX ?= /usr/local
BINDIR ?= bin
DESTDIR ?=

PKGS := \
	x11 xfixes xinerama xrandr \
	wayland-client cairo

RM = rm

SOURCES := $(wildcard src/*.c)
OBJECTS = $(SOURCES:src/%.c=obj/%.o)

GENERATORS := $(wildcard src/*.cgen)
OBJECTS += $(GENERATORS:src/%.cgen=obj/%.o)

NAME := $(shell uname -s)
CFLAGS := \
	$(CFLAGS) \
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
	@echo "  CC\t" $(<)
	@$(CC) -c $(<) -o $(@) $(CFLAGS)

%.c: %.cgen
	@echo " GEN\t" $(<)
	@sh $(<) > $(@)

$(BINARY): $(OBJECTS)
	@echo "LINK\t" $(^)
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
