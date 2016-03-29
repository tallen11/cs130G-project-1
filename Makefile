CC=clang
DEBUG=-g
CFLAGS=-Wall -Wextra -pedantic -std=c11 $(DEBUG)
SRC=tone.o wavefile.o sound.o

all: $(SRC)
	$(CC) $(SRC) -o bin/sound -lm

tone.o: tone.c tone.h
	$(CC) tone.c $(CFLAGS) -c

wavefile.o: wavefile.c wavefile.h
	$(CC) wavefile.c $(CFLAGS) -c

sound.o: sound.c
	$(CC) sound.c $(CFLAGS) -c

clean:
	rm -rf *.o
	rm -rf bin/sound
	rm -rf *.dSYM