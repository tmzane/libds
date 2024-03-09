.POSIX:

CC         = cc
CFLAGS     = -Wall -Wextra -Wpedantic -Wshadow -DTEST -fsanitize=$(SANITIZERS)
LDFLAGS    = -fsanitize=$(SANITIZERS)
SANITIZERS = address,undefined

test: test_map_oa test_map_sc

test_map_oa: map_oa.o map_test.o
	$(CC) $(LDFLAGS) map_oa.o map_test.o && ./a.out

test_map_sc: map_sc.o map_test.o
	$(CC) $(LDFLAGS) map_sc.o map_test.o && ./a.out

map_oa.o: map_oa.c map.h
map_sc.o: map_sc.c map.h
map_test.o: map_test.c map.h

clean:
	rm *.o
	rm a.out
