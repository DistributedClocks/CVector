CC=gcc
CFLAGS=-I.
DEPS = ../cvec/cvec.h  ../cvec/vclock/vlock.h ../cvec/cwpack/src/*.h ../cvec/mpack/src/mpack/*.h

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

simplemake: client_server.o ../cvec/cvec.o ../cvec/vclock/vclock.o ../cvec/cwpack/src/cwpack.o ../cvec/mpack/src/mpack/mpack.o
	gcc -o client_server client_server.o ../cvec/cvec.o ../cvec/vclock/vclock.o ../cvec/cwpack/src/cwpack.o  ../cvec/mpack/src/mpack/mpack.o -I.
