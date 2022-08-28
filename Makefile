CC ?= clang
CFLAGS ?= -Og -Wall -Wpedantic -Wextra -Isrc
PREFIX ?= /usr/local
BINDIR ?= bin
DESTDIR ?=
backends ?= wayland x11

<< := @echo
PKGS := cairo

ifneq ($(shell eval 'echo -e'),-e)
	<< += -e
endif

<<backends>> = $(sort $(filter x11 wayland,$(backends)))
ifeq ($(filter x11,$(<<backends>>)),x11)
	PKGS += x11 xfixes xinerama xrandr
else
	CFLAGS += -DNO_X11
endif

ifeq ($(filter wayland,$(<<backends>>)),wayland)
	PKGS += wayland-client
else
	CFLAGS += -DNO_WAYLAND
endif

<<sources>> := \
	$(wildcard src/*.c) \
	$(foreach <<backend>>,$(<<backends>>),$(wildcard src/$(<<backend>>)/*.c))

<<generators>> := \
	$(wildcard src/*.cgen) \
	$(foreach <<backend>>,$(<<backends>>),$(wildcard src/$(<<backend>>)/*.cgen))

<<hgenerators>> := \
	$(wildcard src/*.hgen) \
	$(foreach <<backend>>,$(<<backends>>),$(wildcard src/$(<<backend>>)/*.hgen))

<<objects>> := $(<<sources>>:src/%.c=obj/%.o)
<<objects>> += $(<<generators>>:src/%.cgen=obj/%.o)

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
	$(<<) "  CC\t" $(<:src/%=%)
	@mkdir -p $(shell dirname $(@))
	@$(CC) -c $(<) -o $(@) $(CFLAGS)


%.h: %.hgen
	$(<<) " GEN\t" $(@:src/%=%)
	@sh -- $(<) $(@)

%.c: %.cgen
	$(<<) " GEN\t" $(@:src/%=%)
	@sh -- $(<) $(@)

$(BINARY): $(<<objects>>)
	$(<<) "LINK\t" $(^:obj/%=%)
	@$(CC) $(^) -o $(@) $(LDFLAGS)

install: $(BINARY)
	install -Dm0755 $(BINARY) $(DESTDIR)$(PREFIX)/$(BINDIR)/$(BINARY)

uninstall:
	$(RM) -f $(DESTDIR)$(PREFIX)/$(BINDIR)/$(BINARY)

clean:
	$(<<) "  RM\t" $(<<objects>>:obj/%=%) $(BINARY)
	@$(RM) -f $(<<objects>>) $(BINARY) obj/.enabled

test: $(BINARY)
	./$(BINARY)

obj/activate_linux.o: obj/.enabled
obj/wayland/wayland.o: src/wayland/wlr-layer-shell-unstable-v1.h

obj/.enabled: .REBUILD
	@test -f $(@) || touch $(@)
	@grep -Fqx "$(<<backends>>)" $(@) || echo "$(<<backends>>)" > $(@)

.PHONY: all clean install uninstall test .REBUILD
.INTERMEDIATE: $(<<hgenerators>>:%.hgen=%.h) $(<<generators>>:%.cgen=%.c)
