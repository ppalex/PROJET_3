CC=gcc
CFLAGS=-Wall -Werror -std=c99

all: record extract

record: record.c record.h
	$(CC) $(CFLAGS) -o record record.c

extract: extract.c extract.h
	$(CC) $(CFLAGS) -o extract extract.c
