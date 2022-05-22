CC=gcc
CFLAGS=-fsanitize=address -Wall -Werror -std=gnu11 -g -lm

tests: tests.c virtual_alloc.c libcmocka-static.a
	$(CC) $(CFLAGS) $^ -o $@

run_tests: tests.c virtual_alloc.c libcmocka-static.a
	$(CC) $(CFLAGS) $^ -o $@
