
CFLAGS += -std=c11 -Wall -Wextra -O3 -DNDEBUG -march=native
LDFLAGS = -lm


all: gibprime test_erat

test_erat: test_erat.o erat.o

gibprime: gibprime.o erat.o

%: %.o
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

clean:
	$(RM) *.o

distclean: clean
	$(RM) gibprime test_erat

.PHONY: all clean distclean

