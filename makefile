MAKE_DIR := $(PWD)

TEST_DIR    := $(MAKE_DIR)/src/test
EXAMPLE_DIR := $(MAKE_DIR)/example

DEPS = src/cvec.h src/vclock/vclock.h src/mpack/mpack.h
SOURCES := $(patsubst %.h,%.c, $(DEPS))
OBJECTS := $(patsubst %.h,%.o, $(notdir $(DEPS)))

CC = gcc
CFLAGS += -c -Wextra -Wall -Wall -Wshadow -Wpointer-arith -Wcast-qual
CFLAGS += -std=c11 -pedantic
# CFLAGS += -Wstrict-prototypes -Wmissing-prototypes
LDFLAGS :=


export MAKE_DIR CC CFLAGS LDFLAGS

all:
	# @$(MAKE) -C $(TEST_DIR) # Temporarily disabled
	@$(MAKE) -C $(EXAMPLE_DIR)

.PHONY: clean
clean:
	#@$(MAKE) -C $(TEST_DIR) clean
	@$(MAKE) -C $(EXAMPLE_DIR) clean


lib: libcvec.a

cvec.o: src/cvec.c $(DEPS)
		@$(CC) $(CFLAGS) $(SOURCES)

libcvec.a: cvec.o
		@ar rcs src/libcvec.a $(OBJECTS)
		@ranlib src/libcvec.a
		@rm -f *.o
		@rm -f src/*.gch

libclean:
		@rm -f *.o *.a src/*.o src/*.gch