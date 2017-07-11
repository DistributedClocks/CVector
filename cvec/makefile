CC=gcc
CFLAGS=-I.
DEPS = cvec.h  ./vclock/vlock.h ./cwpack/src/*.h ./mpack/src/mpack/*.h

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

simplemake: cvec_test.o cvec.o ./vclock/vclock.o ./cwpack/src/cwpack.o ./mpack/src/mpack/mpack.o
	gcc -o cvec_test cvec_test.o cvec.o ./vclock/vclock.o ./cwpack/src/cwpack.o ./mpack/src/mpack/mpack.o -I.
