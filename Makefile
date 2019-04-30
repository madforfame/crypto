CC=gcc
CFLAGS=

all:
	$(CC) cbc.c speck32.c speck32.h tczero.h -o cbc

clean:
	rm *.o


	