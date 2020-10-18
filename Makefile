CC=gcc
CFLAGS=-Werror=vla -Wextra -Wall -Wshadow -Wswitch-default -std=c11
CFLAG_SAN=$(CFLAGS) -fsanitize=address
DEPS=
OBJ=hashmap.o

unittests: main.c hashmap.o
	$(CC) -o unittests main.c hashmap.o

hashmap.o: hashmap.c hashmap.h $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)
	
clean:
	rm *.o unittests
