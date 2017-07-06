CC=gcc
CFLAGS=-I.
DEPS = vclock.h uthash/*.h

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

hellomake: clock_test.o vclock.o 
	gcc -o clock_test clock_test.o vclock.o -I.
