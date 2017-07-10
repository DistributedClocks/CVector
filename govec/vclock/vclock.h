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
	char id[30];
	uint64_t time;
	UT_hash_handle hh;         /* makes this structure hashable */
};
struct vectorClock *clockInit(char * c_id);
void tick(struct vectorClock **vc, char * c_id);
void set(struct vectorClock **vc, char * c_id, uint64_t ticks);
struct vectorClock * copy(struct vectorClock * vc);
void printVC (struct vectorClock *vc);
char * returnVCString(struct vectorClock *vc);
uint64_t findTicks(struct vectorClock *vc, char * c_id);
void merge(struct vectorClock *vc, struct vectorClock *other);