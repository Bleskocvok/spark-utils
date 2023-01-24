
# CFLAGS += -std=c99 -Wall -Wextra -O2
CFLAGS += -std=c99 -Wall -Wextra -O2 -march=native

all: getrand evaluate

getrand: getrand.c

evaluate: evaluate.c

%: %.c
	$(CC) $(CFLAGS) $^ -o $@

clean:
	$(RM)

distclean: clean
	$(RM) getrand evaluate

.PHONY: all clean distclean
