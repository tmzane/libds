.POSIX:

CC         = cc
CFLAGS     = -Wall -Wextra -Wpedantic -Wshadow -fsanitize=$(SANITIZERS)
LDFLAGS    = -fsanitize=$(SANITIZERS)
SANITIZERS = address,undefined

test: map.o map_test.o
	$(CC) $(LDFLAGS) map.o map_test.o && ./a.out

map.o: map.c map.h
map_test.o: map_test.c map.h

clean:
	rm *.o
	rm a.out
