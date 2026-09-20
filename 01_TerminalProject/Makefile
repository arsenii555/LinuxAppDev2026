CC      = cc
CFLAGS  = -std=c11 -O2 -Wall -Wextra -Wpedantic -D_POSIX_C_SOURCE=200809L
LDFLAGS =
LDLIBS  = -lncurses

all: Show

Show: Show.o
	$(CC) $(LDFLAGS) -o $@ $^ $(LDLIBS)

Show.o: Show.c

clean:
	rm -f Show Show.o

.PHONY: all clean