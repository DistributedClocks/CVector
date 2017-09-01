/**
 *The MIT License (MIT)
 *
 *Copyright (c) 2017-2018 University of British Columbia
 *
 *Permission is hereby granted, free of charge, to any person obtaining a copy
 *of this software and associated documentation files (the "Software"), to deal
 *in the Software without restriction, including without limitation the rights
 *to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 *copies of the Software, and to permit persons to whom the Software is
 *furnished to do so, subject to the following conditions:
 *
 *The above copyright notice and this permission notice shall be included in all
 *copies or substantial portions of the Software.
 *
 *THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 *OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 *SOFTWARE.
 *
 */

#include "vclock.h"


int id_sort(struct vectorClock *a, struct vectorClock *b);


struct vectorClock *clock_init(char *c_id) {
    struct vectorClock *newClock = NULL;
    struct vectorClock *clock = (struct vectorClock*)malloc(sizeof(struct vectorClock));
    strncpy(clock->id, c_id, VC_ID_LENGTH);
    clock->time = 0;
    HASH_ADD_STR(newClock, id, clock);
    return newClock;
}

void tick(struct vectorClock **vc, char *c_id) {
    struct vectorClock *clock;

    HASH_FIND_STR(*vc, c_id, clock);  /* id already in the hash? */
    if (clock==NULL) {
        clock = (struct vectorClock*)malloc(sizeof(struct vectorClock));
        strncpy(clock->id, c_id, VC_ID_LENGTH);
        clock->time = 0;
        HASH_ADD_STR(*vc, id, clock);  /* id: name of key field */
    }
    clock->time = clock->time + 1;
}

void set(struct vectorClock **vc, char *c_id, uint64_t ticks) {
    struct vectorClock *clock;

    HASH_FIND_STR(*vc, c_id, clock);  /* id already in the hash? */
    if (clock==NULL) {
        clock = (struct vectorClock *)malloc(sizeof(struct vectorClock));
        strncpy(clock->id, c_id, VC_ID_LENGTH);
        HASH_ADD_STR(*vc, id, clock);  /* id: name of key field */
    }
    clock->time = ticks;
}

struct vectorClock *copy(struct vectorClock *vc) {
    struct vectorClock *vc_copy= NULL;
    struct vectorClock *clock;

    for(clock=vc; clock != NULL; clock=(struct vectorClock *)(clock->hh.next)) {
        struct vectorClock *tmp = (struct vectorClock *) malloc(sizeof(struct vectorClock));
        strncpy(tmp->id,  clock->id, VC_ID_LENGTH);
        tmp->time = clock->time;
        HASH_ADD_STR(vc_copy, id, tmp);  // id: name of key field
    }

    return vc_copy;
}

int64_t find_ticks(struct vectorClock *vc, char *c_id) {
    struct vectorClock *clock;
    HASH_FIND_STR(vc, c_id, clock);
    if (clock==NULL)
        return -1;
    return clock->time;
}

uint64_t last_update(struct vectorClock *vc) {
    uint64_t last = 0;
    struct vectorClock *clock;
    for(clock=vc; clock != NULL; clock=(struct vectorClock*)(clock->hh.next)) {
        if(clock->time > last)
            last = clock->time;
    }
    return clock->time;
}

void merge(struct vectorClock *vc, struct vectorClock *other) {
    struct vectorClock *o_clock;
    
    for(o_clock=other; o_clock != NULL; o_clock=(struct vectorClock *)(o_clock->hh.next)) {
        struct vectorClock *clock;
        HASH_FIND_STR(vc, o_clock->id, clock);  /* id already in the hash? */
        if (clock==NULL) {
            clock = (struct vectorClock *)malloc(sizeof(struct vectorClock));
            strncpy(clock->id,  o_clock->id, VC_ID_LENGTH);
            clock->time = o_clock->time;
            HASH_ADD_STR(vc, id, clock);  /* id: name of key field */
        } else {
            if (clock->time < o_clock->time)
                clock->time = o_clock->time;
        }
    }
}

int id_sort(struct vectorClock *a, struct vectorClock *b) {
    return strcmp(a->id,b->id);
}

int time_sort(struct vectorClock *a, struct vectorClock *b) {
    return (a->time < b->time) ? -1 : (a->time > b->time);
}

void sort_by_id(struct vectorClock **vc) {
    HASH_SORT(*vc, id_sort);
}

void sort_by_time(struct vectorClock **vc) {
    HASH_SORT(*vc, time_sort);
}

struct vectorClock *copy_sort(struct vectorClock *vc) {
    struct vectorClock *vc_sort = copy(vc);
    HASH_SORT(vc_sort, id_sort);
    return vc_sort;
}

/**TODO: Decide if we need sorting here.**/
char *return_vc_string(struct vectorClock *vc) {
    int num_clocks = HASH_COUNT(vc);

    char *vcString = malloc(num_clocks *(20 + 20 + 4) + 2);
    struct vectorClock *clock;
    //sort_by_id(&vc);
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

void print_vc(struct vectorClock *vc) {
    char *output = return_vc_string(vc);
    printf("%s\n",output);
    free(output);
}

/**TODO: Figure out requirements for vector clock comparison.**/
/*int compare(struct vectorClock *vc, struct vectorClock *other, int condition) {
    return 0;
}*/


