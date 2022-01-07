# you should not need to modify this.
CC =gcc
CFLAGS = -Og -g -Wall

all: string-to-char-array clone

clone:
	./string-to-char-array < seed.c > replicate.c
	cat seed.c >> replicate.c
	$(CC) replicate.c -o replicate
	./replicate > kid.c
	diff replicate.c kid.c

string-to-char-array: string-to-char-array.c
	$(CC) $(CFLAGS) string-to-char-array.c -o string-to-char-array

clean:
	rm -f string-to-char-array replicate replicate.c kid.c *~
