CC=gcc
CFLAGS=-I.
DEPS = govec.h ./vclock/uthash/*.h

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

simplemake: govec_test.o govec.o 
	gcc -o govec_test govec_test.o govec.o -I.
