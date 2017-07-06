#include "vclock.h"

/*struct vectorClock {
  unsigned int id;
  unsigned int time;
  struct vectorClock * next;
};*/

char * returnVCString(struct vectorClock *vc);
int id_sort(struct vectorClock *a, struct vectorClock *b);
struct vectorClock * sort_by_id(struct vectorClock *vc);


//returns a new vector clock
struct vectorClock * new(pid_t id) {
    struct vectorClock *newClock = NULL;
    struct vectorClock *clock = (struct vectorClock*)malloc(sizeof(struct vectorClock));
    clock->id = id;
    clock->time = 0;
    HASH_ADD_INT(newClock, id, clock);
    return newClock;
}

void tick(struct vectorClock *vc, pid_t c_id) {
    struct vectorClock *clock;

    HASH_FIND_INT(vc, &c_id, clock);  /* id already in the hash? */
    if (clock==NULL) {
        clock = (struct vectorClock*)malloc(sizeof(struct vectorClock));
        clock->id = c_id;
        clock->time = 0;
        HASH_ADD_INT(vc, id, clock);  /* id: name of key field */
    } else {
        clock->time = clock->time + 1;
    }
}

void set(struct vectorClock *vc, pid_t c_id, pid_t ticks) {
    struct vectorClock *clock;

    HASH_FIND_INT(vc, &c_id, clock);  /* id already in the hash? */
    if (clock==NULL) {
        clock = (struct vectorClock*)malloc(sizeof(struct vectorClock));
        clock->id = ticks;
        clock->time = 0;
        HASH_ADD_INT(vc, id, clock );  /* id: name of key field */
    } else {
        clock->id = ticks;
    }
}

struct vectorClock * copy(struct vectorClock * vc) {
    struct vectorClock *vc_copy= NULL;
    struct vectorClock *clock;

    for(clock=vc; clock != NULL; clock=(struct vectorClock*)(clock->hh.next)) {
        struct vectorClock *tmp = (struct vectorClock*)malloc(sizeof(struct vectorClock));
        tmp->id = clock->id;
        tmp->time = clock->time;
        printf("clock id %d: time %d\n", tmp->id, tmp->time);
        HASH_ADD_INT(vc_copy, id, tmp);  // id: name of key field
    }

    return vc_copy;
}

void printVC(struct vectorClock *vc) {
    char * output = returnVCString(vc);
    printf("Output:\n%s\n",output);
    free(output);
}


int id_sort(struct vectorClock *a, struct vectorClock *b) {
    return (a->id < b->id) ? -1 : (a->id > b->id);
}
struct vectorClock * sort_by_id(struct vectorClock *vc) {
    struct vectorClock *vc_sort = copy(vc);
    HASH_SORT(vc_sort, id_sort);
    return vc_sort;
}

char * returnVCString(struct vectorClock *vc) {
    int num_clocks = HASH_COUNT(vc);

    unsigned int pidLength = sizeof(pid_t);
    char * vcString = malloc(num_clocks*(pidLength + 10) + 2);
    struct vectorClock *clock;
    struct vectorClock * vc_sort = sort_by_id(vc);
    int len = sprintf(vcString, "{");
    int i = 1;
    for(clock=vc_sort; clock != NULL; clock=(struct vectorClock*)(clock->hh.next)) {
        len += sprintf(vcString+len, "\"%d\":%u", clock->id, clock->time);
        if(i < num_clocks)
            len += sprintf(vcString+len, ", ");
        i++;
    }
    for(clock=vc; clock != NULL; clock=(struct vectorClock*)(clock->hh.prev)) {
    }
    len = sprintf(vcString+len, "}");
    return vcString;
}