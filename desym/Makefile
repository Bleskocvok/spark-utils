

CFLAGS += -std=c99 -Wall -Wextra -pedantic -O2


all: desym

desym: desym.o

clean:
	$(RM) *.o

distclean: clean
	$(RM) desym
	$(RM) long*
	$(RM) loop*

.PHONY: all clean distclean

