# CC = gcc
# CFLAGS += -c -Wextra -Wall -g3 -std=gnu11
LDFLAGS += -L./ -lcheck -lsubunit -lpthread -lrt -lm
DEPS = ../vclock/vclock.h ../mpack/mpack.h 
app = vclock_test cvec_test

objects := $(patsubst %.h,%.o, $(DEPS))

all: $(app)
	@-rm -r $(objects)
	@-rm -r ../cvec.o

vclock_test: $(objects)
	@$(CC) -o $@ $@.c $(objects) $(LDFLAGS)

cvec_test: ../cvec.o
	@$(CC) -o $@ $@.c $(LDFLAGS) -L../ -lcvec

%.o: %.c $(DEPS)
	@$(CC) $(CFLAGS) -o $@ $<

clean:
	@echo "Cleaning..."
	@-rm -r $(app)
	@-rm -r *.o