MAKE_DIR = $(PWD)

CVEC_DIR    := $(MAKE_DIR)/src/ 
VCLOCK_DIR  := $(MAKE_DIR)/src/vclock
EXAMPLE_DIR := $(MAKE_DIR)/example

DEPS = src/cvec.h src/vclock/vclock.h src/mpack/mpack.h
SOURCES := $(patsubst %.h,%.c, $(DEPS))
OBJECTS := $(patsubst */%.h,%.o, $(DEPS))

CC = gcc
CFLAGS += -c -Wextra -Wall -Wall -Wshadow -Wpointer-arith -Wcast-qual
CFLAGS += -m64 -std=gnu11 -pedantic
# CFLAGS += -Wstrict-prototypes -Wmissing-prototypes
LDFLAGS :=


export MAKE_DIR CC CFLAGS LDFLAGS

all:
	@$(MAKE) -C $(CVEC_DIR)
	@$(MAKE) -C $(VCLOCK_DIR)
	@$(MAKE) -C $(EXAMPLE_DIR)


.PHONY: clean
clean:
	@$(MAKE) -C $(CVEC_DIR) clean
	@$(MAKE) -C $(VCLOCK_DIR) clean
	@$(MAKE) -C $(EXAMPLE_DIR) clean


lib: libcvec.a
MAKE_DIR = $(PWD)

all: libcvec.a

cvec.o: src/cvec.c $(DEPS)
		@$(CC) $(CFLAGS) $(SOURCES)

libcvec.a: cvec.o
		ar rcs src/libcvec.a $(OBJECTS)
		ranlib src/libcvec.a
		rm -f *.o
		rm -f src/*.gch

libclean:
		rm -f *.o *.a src/*.o src/*.gch


# srcExt = c
# srcDir = .
# objDir = obj
# binDir = bin
# libDir = lib
# excludes = 'mpack/test|cvec_test|clock_test|client_server'
# app := src/cvec_test vclock/clock_test ../example/client_server

# CC = gcc
# CFlags += -c -Wextra -Wall -g3 -std=gnu11
# LDFlags := 

# sources := $(shell find "$(srcDir)" -name '*.$(srcExt)' | grep -Ev $(excludes) ) 
# srcDirs := $(shell find . -name '*.$(srcExt)' -exec dirname {} \; | uniq)
# objects := $(patsubst %.$(srcExt),$(objDir)/%.o,$(sources) )

# all: $(app)

# $(app):  % : $(binDir)/%

# $(binDir)/%: buildrepo $(objects)
# 	@mkdir -p `dirname $@`
# 	@$(eval appObject = $(@:bin/%=%))
# 	@echo "Objects: $(objects)"
# 	@echo "Linking $@"
# 	$(CC) $(srcDir)/$(appObject).c -o $@ $(objects) $(LDFlags)

# $(objDir)/%.o: %.$(srcExt)
# 	@echo "Sources: $(sources)"
# 	@echo "SrcDirs: $(srcDirs)"
# 	@echo "Objects: $(objects)\n"
# 	@echo "Building $@ ..."
# 	@echo "Compiling $< ..."
# 	$(CC) $(CFlags) $< -o $@

# clean:
# 	@echo "Cleaning..."
# 	-rm -r $(objDir)
# 	-rm -r $(binDir)/*

# buildrepo:
# 	@$(call make-repo)

# define make-repo
#    for dir in $(srcDirs);\
#    do\
# 	mkdir -p $(objDir)/$$dir;\
#    done
# endef