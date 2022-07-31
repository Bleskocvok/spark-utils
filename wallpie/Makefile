
CFLAGS ?= -std=c99 -Wall -Wextra -O2

SRC = wallpie.c

all: wallpie

wallpie: $(SRC)
	$(CC) $(CFLAGS) $^ -o $@

distclean:
	$(RM) wallpie

