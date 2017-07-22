# CC = gcc
# CFLAGS += -c -Wextra -Wall -g3 -std=c11
# LDFlags := 
DEPS = ../src/cvec.h ../src/vclock/vclock.h ../src/mpack/mpack.h
app := client_server basic_example
lib = ../src/libcvec.a

objects := $(patsubst %.h,%.o, $(DEPS))


all: $(app)
	# @-rm -r $<.o
	@-rm -r $(objects)

$(app): $(objects)
	$(CC) -o $@ $@.c $(objects) $(LDFLAGS)

%.o: %.c $(DEPS)
	@$(CC) $(CFLAGS) -o $@ $<

clean:
	@echo "Cleaning..."
	@-rm -r $(app)
	@-rm -r *.o

libmake:
	@$(CC) -o $(app) $(app).o $(lib) $(LDFLAGS)