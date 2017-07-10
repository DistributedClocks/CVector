#include "vclock.h"

/*struct vectorClock {
    pid_t id;
    uint32_t time;
    UT_hash_handle hh;          
};*/

char * returnVCString(struct vectorClock *vc);
int id_sort(struct vectorClock *a, struct vectorClock *b);
struct vectorClock * sort_by_id(struct vectorClock *vc);


//returns a new vector clock
struct vectorClock * clockInit(char * c_id) {
    struct vectorClock *newClock = NULL;
    struct vectorClock *clock = (struct vectorClock*)malloc(sizeof(struct vectorClock));
    strcpy(clock->id,  c_id);
    clock->time = 0;
    HASH_ADD_STR(newClock, id, clock);
    return newClock;
}

void tick(struct vectorClock **vc, char * c_id) {
    struct vectorClock *clock;

    HASH_FIND_STR(*vc, c_id, clock);  /* id already in the hash? */
    if (clock==NULL) {
        clock = (struct vectorClock*)malloc(sizeof(struct vectorClock));
        strcpy(clock->id,  c_id);
        clock->time = 0;
        HASH_ADD_STR(*vc, id, clock);  /* id: name of key field */
    }
    clock->time = clock->time + 1;
}
void set(struct vectorClock **vc, char * c_id, uint64_t ticks) {
    struct vectorClock *clock;

    HASH_FIND_STR(*vc, c_id, clock);  /* id already in the hash? */
    if (clock==NULL) {
        clock = (struct vectorClock*)malloc(sizeof(struct vectorClock));
        strcpy(clock->id,  c_id);
        HASH_ADD_STR(*vc, id, clock);  /* id: name of key field */
    }
    clock->time = ticks;
}
uint64_t findTicks(struct vectorClock *vc, char * c_id) {
    struct vectorClock *clock;
    HASH_FIND_STR(vc, c_id, clock);
    if (clock==NULL)
        return -1;
    return clock->time;
}

uint64_t lastUpdate (struct vectorClock *vc, uint64_t last) {
    struct vectorClock *clock;
    for(clock=vc; clock != NULL; clock=(struct vectorClock*)(clock->hh.next)) {
        if(clock->time > last)
            last = clock->time;
    }
    return clock->time;
}

void merge(struct vectorClock *vc, struct vectorClock *other) {
    struct vectorClock *o_clock;
    
    for(o_clock=other; o_clock != NULL; o_clock=(struct vectorClock*)(o_clock->hh.next)) {
        struct vectorClock *clock;
        HASH_FIND_STR(vc, o_clock->id, clock);  /* id already in the hash? */
        if (clock==NULL) {
            clock = (struct vectorClock*)malloc(sizeof(struct vectorClock));
            strcpy(clock->id,  o_clock->id);
            clock->time = o_clock->time;
            HASH_ADD_STR(vc, id, clock);  /* id: name of key field */
        } else {
            if (clock->time < o_clock->time)
                clock->time = o_clock->time;
        }
    }
}

struct vectorClock * copy(struct vectorClock * vc) {
    struct vectorClock *vc_copy= NULL;
    struct vectorClock *clock;

    for(clock=vc; clock != NULL; clock=(struct vectorClock*)(clock->hh.next)) {
        struct vectorClock *tmp = (struct vectorClock*)malloc(sizeof(struct vectorClock));
        strcpy(tmp->id,  clock->id);
        tmp->time = clock->time;
        HASH_ADD_STR(vc_copy, id, tmp);  // id: name of key field
    }

    return vc_copy;
}

void printVC(struct vectorClock *vc) {
    char * output = returnVCString(vc);
    printf("Output:\n%s\n",output);
    free(output);
}
/**TODO: Decide if we need sorting here.**/

char * returnVCString(struct vectorClock *vc) {
    int num_clocks = HASH_COUNT(vc);

    char * vcString = malloc(num_clocks * (20 + 20 + 4) + 2);
    struct vectorClock *clock;
/*    struct vectorClock * vc_sort = sort_by_id(vc);*/
    int len = sprintf(vcString, "{");
    int i = 1;
    for(clock=vc; clock != NULL; clock=(struct vectorClock*)(clock->hh.next)) {
        len += sprintf(vcString+len, "\"%s\":%lu", clock->id, clock->time);
        if(i < num_clocks)
            len += sprintf(vcString+len, ", ");
        i++;
    }
    len = sprintf(vcString+len, "}");
    return vcString;
}

int id_sort(struct vectorClock *a, struct vectorClock *b) {
    return (a->id < b->id) ? -1 : (a->id > b->id);
}
struct vectorClock * sort_by_id(struct vectorClock *vc) {
    struct vectorClock *vc_sort = copy(vc);
    HASH_SORT(vc_sort, id_sort);
    return vc_sort;
}

/**TODO: Figure out requirements for vector clock comparison.**/
int compare(struct vectorClock *vc, struct vectorClock *other, int condition) {
    return 0;
}


