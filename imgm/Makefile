
CFLAGS = -std=c99 -Wall -Wextra -pedantic -O2

LDLIBS = -lm

SRC = imgm.c deps/stb_image.c
OBJ = $(SRC:.c=.o)


all: imgm

imgm: $(OBJ)

clean:
	$(RM) $(OBJ)

distclean: clean
	$(RM) imgm

.PHONY: all clean distclean
