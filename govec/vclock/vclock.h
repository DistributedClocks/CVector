#include <stdbool.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include "uthash/uthash.h"
struct vectorClock {
	pid_t id;
	uint32_t time;
	UT_hash_handle hh;         /* makes this structure hashable */
};
struct vectorClock * new(pid_t id);
void tick(struct vectorClock *vc, pid_t id);
void set(struct vectorClock *vc, pid_t id, pid_t ticks);
struct vectorClock * copy(struct vectorClock * vc);
void printVC (struct vectorClock *vc);