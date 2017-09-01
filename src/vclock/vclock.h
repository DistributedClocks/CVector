/**
 * The MIT License (MIT)
 * 
 * Copyright (c) 2017 Distributed Clocks
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to 
 * deal in the Software without restriction, including without limitation the 
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or 
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in 
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING 
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER 
 * DEALINGS IN THE SOFTWARE.
 * 
 */
#ifndef _HEADER_vclock_
#define _HEADER_vclock_

#include <stdio.h> //printf
#include "uthash/uthash.h"

/* The maximum size of a vector clock id. Can freely be modified. */
#ifndef VC_ID_LENGTH
#define VC_ID_LENGTH 30
#endif

/* The maximum size of a single vector clock. Depended on VC_ID_LENGTH.*/
#ifndef CLOCKSIZE
#define CLOCKSIZE VC_ID_LENGTH + 2 * 8
#endif


/**
 * The vector clock map type. 
 * "id" is a string of pre-specified fixed length.
 * "time" is a 64 bit integer denoting the current time value of a clock.
 * "hh" is a uthash extension which makes this data structure dynamic and 
 * hashable.
 */
struct vectorClock {
	char id[VC_ID_LENGTH];
	uint64_t time;
	UT_hash_handle hh;
};

/**
 * Returns a new vector clock map initialised with the given process id.
 * The time value of the vector clock structure contained is set to 0.
 * The given char is the key of the clock, the time variable is the value.
 * If the length of c_id exceeds VC_ID_LENGTH, the id name will be truncated.
 * As hash map the uthash library is used, which allows for efficient and
 * scalable manipulation of C.
 *
 * @param c_id The process id as string representation.
 */
struct vectorClock *clock_init(char *c_id);

/**
 * Increments the time value of the vector clock "c_id" by one.
 * If the specified id does not exist in the map, a new clock with a value of
 * one will be created.
 * To correctly increment the value of the vector map it is required to pass 
 * the address of the vector map.
 *
 * @param vc The address of the current vector clock map.
 * @param c_id The process id as string representation.
 */
void tick(struct vectorClock **vc, char *c_id);

/**
 * Sets the time value of the vector clock "c_id" to the value "ticks".
 * If the specified id does not exist in the map, a new clock with a value of
 * "ticks" will be created.
 * To correctly increment the value of the vector map it is required to pass 
 * the address of the vector map.
 *
 * @param vc The address of the current vector clock map.
 * @param c_id The process id as string representation.
 * @param ticks The value of time to be set as.
 */
void set(struct vectorClock **vc, char *c_id, uint64_t ticks);

/**
 * Returns a copy of the vector clock map. Both clock maps remain valid.
 *
 * @param vc The current vector clock map.
 */
struct vectorClock *copy(struct vectorClock *vc);

/**
 * Returns the current time value of the clock "c_id". If this id does not 
 * exist, a negative value is returned.
 *
 * @param vc The current vector clock map.
 * @param c_id The process id as string representation.
 */
int64_t find_ticks(struct vectorClock *vc, char *c_id);

/**
 * Returns the most recent update of all the clocks contained in the map.
 * In this context, update means the current highest time value across all 
 * clocks.
 *
 * @param vc The current vector clock map.
 */
uint64_t last_update(struct vectorClock *vc);

/**
 * Merges the clock map "vc" with a second clock map "other". This operation 
 * directly modifies "vc" and will result in "vc" encapsulating "other".
 * If both maps contain the same specific id, the higher time value will be 
 * chosen.
 *
 * @param vc The current vector clock map.
 * @param other The vector clock map to merge with.
 */
void merge(struct vectorClock *vc, struct vectorClock *other);

/**
 * Sorts a clock map based on the lexical rank of the contained ids.
 * This operation directly modifies the ordering of the vector clock.
 *
 * @param vc The address of the current vector clock map.
 */
void sort_by_id(struct vectorClock **vc);

/**
 * Sorts a clock map based on the numerical rank of contained time values.
 * This operation directly modifies the ordering of the vector clock.
 *
 * @param vc The address of the current vector clock map.
 */
void sort_by_time(struct vectorClock **vc);

/**
 * Sorts a clock map based on the lexical rank of the contained ids.
 * This operation returns a copy of the vector clock and does not modify the
 * original.
 *
 * @param vc The current vector clock map.
 */
struct vectorClock *copy_sort(struct vectorClock *vc);

/**
 * Returns a string representation of the vector map in the following format:
 * {"ProcessID 1": Time1, "ProcessID 2": Time2, ...}
 *
 * @param vc The current vector clock map.
 */
char *return_vc_string(struct vectorClock *vc);

/**
 * Prints the string generated by returnVCString for a given vector map.
 *
 * @param vc The current vector clock map.
 */
void print_vc(struct vectorClock *vc);
#endif