# CC = gcc
# CFLAGS += -c -Wextra -Wall -g3 -std=gnu11
LDFLAGS += -lcheck -lsubunit -lpthread -lrt -lm
DEPS = vclock/vclock.h mpack/mpack.h
app = test/vclock_test test/cvec_test
lib = -llibcvec

objects := $(patsubst %.h,%.o, $(DEPS))

all: $(app)
	@-rm -r $(objects)

test/vclock_test: $(objects)
	@$(CC) -o $@ $@.c $(objects) $(LDFLAGS)

%.o: %.c $(DEPS)
	@$(CC) $(CFLAGS) -o $@ $<

test/cvec_test:
	@$(CC) -o $@ $@.c -L./ -lcvec $(LDFLAGS)

cvec: 
clean:
	@echo "Cleaning..."
	@-rm -r $(app)
	@-rm -r *.o