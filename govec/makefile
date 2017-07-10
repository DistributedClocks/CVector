CC=gcc
CFLAGS=-I.
DEPS = govec.h  ./vclock/vlock.h ./cwpack/src/*.h

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

simplemake: govec_test.o govec.o ./vclock/vclock.o ./cwpack/src/cwpack.o 
	gcc -o govec_test govec_test.o govec.o ./vclock/vclock.o ./cwpack/src/cwpack.o -I.
