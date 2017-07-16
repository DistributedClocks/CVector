# CC = gcc
# CFLAGS += -c -Wextra -Wall -g3 -std=gnu11
# LDFlags := 
DEPS = cvec.h vclock/vclock.h mpack/src/mpack/mpack.h
app = cvec_test

objects := $(patsubst %.h,%.o, $(DEPS))

# %.o: %.c $(DEPS)
# 	$(CC) $(CFlags) -o $@ $< $(CFLAGS)

# simplemake: $(app).o
# 	@echo "Hello $(objects)"
# 	@$(CC) -o $(app) $(app).o 


%.o: %.c $(DEPS)
	@$(CC) $(CFLAGS) -o $@ $<

simplemake: $(app).o $(objects)
	@$(CC) -o $(app) $(app).o $(objects) $(LDFLAGS)
	@-rm -r $(app).o

clean:
	@echo "Cleaning..."
	@-rm -r $(app)
	@-rm -r *.o