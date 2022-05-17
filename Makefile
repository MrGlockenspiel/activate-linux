CC		= clang
LD		= clang
PREFIX		= /usr/local

COMMON		= -Wall -Wextra -Wpedantic
COMMON		+= -O2 -g

# WORD OF ADVICE:
# IF ANYTHING IS CAUSING THE LINKER TO SHIT ITSELF,
# THROW THE BELOW LINE INTO THE ABYSS
COMMON		+= -flto=full

LIBS		= x11 xfixes xinerama cairo
CFLAGS		= $(COMMON) $(shell pkg-config --cflags $(LIBS))
LDFLAGS		= $(COMMON) $(shell pkg-config --libs $(LIBS)) -fuse-ld=lld

BIN_PATH	= bin
OBJ_PATH	= obj
SRC_PATH	= src

CLEAN_LIST	= $(BIN_PATH) $(OBJ_PATH)

SRC		= $(shell find $(SRC_PATH) -name "*.c")
OBJ		= $(addprefix $(OBJ_PATH)/, $(addsuffix .o, $(notdir $(basename $(SRC)))))

NAME	= $(shell uname -s)

ifeq ($(NAME),Linux)
	BINARY_NAME	= activate-linux
endif

ifeq ($(NAME),Darwin)
	BINARY_NAME	= activate-macos
endif

BINARY	= $(BIN_PATH)/$(BINARY_NAME)

default: makedir all

all: $(BINARY)

# Compile
$(OBJ_PATH)/%.o: $(SRC_PATH)/%.c Makefile
	$(CC) $(CFLAGS) -c -o $@ $<

# Link
$(BINARY): $(OBJ) Makefile
	$(LD) $(LDFLAGS) -o $@ $(OBJ)

.PHONY: makedir
makedir:
	mkdir -p $(BIN_PATH) $(OBJ_PATH)

.PHONY: install
install: $(BINARY)
	install -Dm0755 $(BINARY) $(DESTDIR)$(PREFIX)/$(BINDIR)/$(BINARY)

.PHONY: uninstall
uninstall:
	$(RM) -f $(DESTDIR)$(PREFIX)$(BINDIR)/$(BINARY)

.PHONY: strip
strip:
	strip -R .comment* -R .note* -s $(BINARY)

.PHONY: clean
clean:
	rm -rf $(CLEAN_LIST)

.PHONY: run
run: $(BINARY)
	./$(BINARY)
