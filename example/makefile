# CC = gcc
# CFLAGS += -c -Wextra -Wall -g3 -std=c11
# LDFlags := 
DEPS = ../src/cvec.h ../src/vclock/vclock.h ../src/mpack/mpack.h
app = client_server

objects := $(patsubst %.h,%.o, $(DEPS))

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