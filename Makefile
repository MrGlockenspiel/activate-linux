# if $CC is not set, guess default `cc'. It has to be in system
CC ?= cc
# compile options
CFLAGS ?= -Os -Wall -Wpedantic -Wextra
# link options
LDFLAGS ?= -s

# install path is: $(DESTDIR)$(PREFIX)/$(BINDIR)/$(BINARY)
DESTDIR ?=
PREFIX ?= /usr/local
BINDIR ?= bin

MANDIR ?= $(PREFIX)/share/man

# implemented backends: wayland x11 gdi
backends ?= wayland x11

# addons
with     ?= libconfig

# needs rebuild on dependency change (object name, w/o obj/ prefix)
<<needs-rebuild>> = \
	activate_linux.o \
	options.o

IS_CLANG = $(shell $(CC) -v 2>&1 | grep -q clang && echo true)
ifeq ($(IS_CLANG),true)
	CFLAGS += -Wno-gnu-zero-variadic-macro-arguments
	CFLAGS += -Wno-empty-translation-unit
endif

# Echo function
<< := echo
ifneq ($(shell eval 'echo -e'),-e)
	<< += -e
endif

# Mess with backends
<<backends>> = $(sort $(filter x11 wayland gdi,$(backends)))
# optional addons
<<with>>       = $(sort $(filter libconfig,$(with)))
<<addons>>     =
<<addon-srcs>> = src/config.c

ifeq ($(filter x11,$(<<backends>>)),x11)
	PKGS += x11 xfixes xinerama xrandr xext x11
	CFLAGS += -DX11
endif
ifeq ($(filter wayland,$(<<backends>>)),wayland)
	PKGS += wayland-client
	CFLAGS += -DWAYLAND
	LDFLAGS += -lrt
endif
ifneq ($(filter wayland x11,$(<<backends>>)),)
	PKGS += cairo
	CFLAGS += -DCOLOR_HELP -DCAIRO
endif
ifeq ($(filter gdi,$(<<backends>>)),gdi)
# Current toolchain architecture variable from MSYS2 project
	ifeq ($(MSYSTEM_CARCH),i686)
		CFLAGS += -m32
	endif
	CFLAGS += -DGDI
	LDFLAGS += -lgdi32
endif
# rebuild on optional deps change
ifneq ($(filter libconfig,$(<<with>>)),)
	ifneq ($(shell pkg-config --exists libconfig && echo exists),)
		PKGS += libconfig
		CFLAGS += -DLIBCONFIG
		<<addons>> += libconfig
	endif
endif

ifneq ($(PKGS),)
	CFLAGS += $(shell pkg-config --cflags $(PKGS))
	LDFLAGS += $(shell pkg-config --libs $(PKGS))
endif

<<sources>> := \
	$(filter-out $(<<addon-srcs>>), \
	$(wildcard src/*.c) \
	$(foreach <<backend>>,$(<<backends>>),$(wildcard src/$(<<backend>>)/*.c)))

<<generators>> := \
	$(filter-out $(<<addon-srcs>>), \
	$(wildcard src/*.cgen) \
	$(foreach <<backend>>,$(<<backends>>),$(wildcard src/$(<<backend>>)/*.cgen)))

<<hgenerators>> := \
	$(filter-out $(<<addon-srcs>>), \
	$(wildcard src/*.hgen) \
	$(foreach <<backend>>,$(<<backends>>),$(wildcard src/$(<<backend>>)/*.hgen)))

ifneq ($(filter libconfig,$(<<addons>>)),)
	<<sources>> += src/config.c
endif

<<objects>> := $(<<sources>>:src/%.c=obj/%.o)
<<objects>> += $(<<generators>>:src/%.cgen=obj/%.o)

# Output file name
NAME := $(shell uname -s)
ifeq ($(NAME),Linux)
	BINARY ?= activate-linux
endif
ifeq ($(NAME),Darwin)
	BINARY ?= activate-macos
endif
ifneq (, filter($(shell uname -o),Msys Cygwin))
	ifeq ($(MSYSTEM_CARCH),i686)
		BINARY ?= activate-windows.exe
	else
		BINARY ?= activate-windows64.exe
	endif
endif

# Use .$(BINARY).d file to track backend change
<<depends>> = $(sort $(<<backends>>) $(<<addons>>))
ifneq ($(sort $(file < .$(BINARY).d)),$(<<depends>>))
	<<null>> := $(file  > .$(BINARY).d,$(<<depends>>))
endif
undefine <<depends>> <<null>>
# using some makefile sorcery

all: $(BINARY)

obj/%.o: src/%.c
	@$(<<) "  CC\t" $(<:src/%=%)
	@mkdir -p $(shell dirname $(@))
	@$(CC) -c $(<) -o $(@) $(CFLAGS)


%.h: %.hgen
	@$(<<) " GEN\t" $(@:src/%=%)
	@sh -- $(<) $(@)

%.c: %.cgen
	@$(<<) " GEN\t" $(@:src/%=%)
	@sh -- $(<) $(@)

$(BINARY): $(<<objects>>)
	@if test -n "$(<<addons>>)"; then $(<<) "WITH\t" $(<<addons>>);fi
	@$(<<) "LINK\t" "$(BINARY)$(^:obj/%=\\n\\t + %)"
	@$(CC) $(^) -o $(@) $(LDFLAGS)

install: $(BINARY)
	install -Dm0755 $(BINARY) $(DESTDIR)$(PREFIX)/$(BINDIR)/$(BINARY)
	install -Dm0644 activate-linux.1 $(MANDIR)/man1/activate-linux.1

uninstall:
	sudo $(RM) -f $(DESTDIR)$(PREFIX)/$(BINDIR)/$(BINARY)
	sudo $(RM) -f $(MANDIR)/man1/activate-linux.1
	sudo mandb -q

appimage: $(BINARY)
	curl -#L -O https://github.com/linuxdeploy/linuxdeploy/releases/download/continuous/linuxdeploy-x86_64.AppImage
	chmod +x linuxdeploy-x86_64.AppImage
	./linuxdeploy-x86_64.AppImage --appdir AppDir --executable ./$(BINARY) --desktop-file res/activate-linux.desktop --icon-file res/icon.png --output appimage

clean:
	@$(<<) "  RM\t" "$(BINARY)$(<<objects>>:obj/%=\\n\\t + %)"
	@$(RM) -f $(<<objects>>) $(BINARY) .$(BINARY).d

test: $(BINARY)
	./$(BINARY)

$(<<needs-rebuild>>:%=obj/%): .$(BINARY).d
obj/wayland/wayland.o: src/wayland/wlr-layer-shell-unstable-v1.h

.PHONY: all clean install uninstall test
.INTERMEDIATE: $(<<hgenerators>>:%.hgen=%.h) $(<<generators>>:%.cgen=%.c)
