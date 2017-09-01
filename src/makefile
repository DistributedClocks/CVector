# CC = gcc
# CFLAGS += -c -Wextra -Wall -g3 -std=gnu11
LDFLAGS += -lcheck -lsubunit -lpthread -lrt -lm
DEPS = cvec.h vclock/vclock.h mpack/mpack.h
app = test/vclock_test

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
	@-rm -r $(objects)

clean:
	@echo "Cleaning..."
	@-rm -r $(app)
	@-rm -r *.o