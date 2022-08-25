CC ?= clang
CFLAGS ?= -Og -Wall -Wpedantic -Wextra -Isrc
PREFIX ?= /usr/local
BINDIR ?= bin
DESTDIR ?=
x11 := yes
wayland := yes

<< := @echo
PKGS := cairo

ifneq ($(shell eval 'echo -e'),-e)
	<< += -e
endif

ifneq ($(x11),no)
	PKGS += x11 xfixes xinerama xrandr
else
	CFLAGS += -DNO_X11
endif

ifneq ($(wayland),no)
	PKGS += wayland-client
else
	CFLAGS += -DNO_WAYLAND
endif

RM := rm

SOURCES := $(wildcard src/*.c)
GENERATORS := $(wildcard src/*.cgen)
HGENERATORS := $(wildcard src/*.hgen)

ifneq ($(wayland),no)
	SOURCES += $(wildcard src/wayland/*.c)
	GENERATORS += $(wildcard src/wayland/*.cgen)
	HGENERATORS += $(wildcard src/wayland/*.hgen)
endif
ifneq ($(x11),no)
	SOURCES += $(wildcard src/x11/*.c)
	GENERATORS += $(wildcard src/x11/*.cgen)
	HGENERATORS += $(wildcard src/x11/*.hgen)
endif

OBJECTS := $(SOURCES:src/%.c=obj/%.o)
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
	$(<<) "  CC\t" $(<:src/%=%)
	@mkdir -p $(shell dirname $(@))
	@$(CC) -c $(<) -o $(@) $(CFLAGS)

%.h: %.hgen
	$(<<) " GEN\t" $(@:src/%=%)
	@sh -- $(<) $(@)

%.c: %.cgen
	$(<<) " GEN\t" $(@:src/%=%)
	@sh -- $(<) $(@)

$(BINARY): $(OBJECTS)
	$(<<) "LINK\t" $(^:obj/%=%)
	@$(CC) $(^) -o $(@) $(LDFLAGS)

install: $(BINARY)
	install -Dm0755 $(BINARY) $(DESTDIR)$(PREFIX)/$(BINDIR)/$(BINARY)

uninstall:
	$(RM) -f $(DESTDIR)$(PREFIX)/$(BINDIR)/$(BINARY)

clean:
	$(<<) "  RM\t" $(OBJECTS:obj/%=%) $(BINARY)
	@$(RM) -f $(OBJECTS) $(BINARY) obj/.enabled

test: $(BINARY)
	./$(BINARY)

obj/activate_linux.o: obj/.enabled
obj/wayland/wayland.o: src/wayland/wlr-layer-shell-unstable-v1.h

obj/.enabled: .REBUILD
	@test -f $(@) || touch $(@)
	@test "$(x11)" = "no" && grep -q x11 $(@) && sed -i '/x11/d' $(@) || true
	@test "$(x11)" = "no" || grep -q x11 $(@) || (echo 'x11' >> $(@))
	@test "$(wayland)" = "no" && grep -q wayland $(@) && sed -i '/wayland/d' $(@) || true
	@test "$(wayland)" = "no" || grep -q wayland $(@) || (echo 'wayland' >> $(@))

.PHONY: all clean install test .REBUILD
.INTERMEDIATE: $(HGENERATORS:%.hgen=%.h)
