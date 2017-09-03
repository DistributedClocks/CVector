# CC = gcc
# CFLAGS += -c -Wextra -Wall -g3 -std=c11
# LDFlags := 
DEPS = ../src/cvec.h ../src/vclock/vclock.h ../src/mpack/mpack.h
app := client_server basic_example
lib = ../src/libcvec.a
app-lib := $(patsubst %,%-lib, $(app))

objects := $(patsubst %.h,%.o, $(DEPS))

# Compile the program using the default dependencies

all: $(app)
	@-rm -r $(objects)

$(app): $(objects)
	$(CC) -o $@ $@.c $(objects) $(LDFLAGS)

%.o: %.c $(DEPS)
	@$(CC) $(CFLAGS) -o $@ $<

clean:
	@echo "Cleaning..."
	@-rm -r $(app)
	@-rm -r *.o

# Compile the program using the provided library file
libmake: $(app-lib)

%-lib:
	@$(eval name := $(patsubst %-lib,%, $@))
	@$(CC) -o $(name) $(name).c $(lib) $(LDFLAGS)