CC=gcc
CFLAGS=

all:
	$(CC) cbc.c tczero.c -o cbc

clean:
	rm *.o


	