#include "../vclock/vclock.h"

void testBasicInit() {
    struct vectorClock *n = NULL;
    tick(&n, "a");
    tick(&n, "b");
    int result = findTicks(n, "a");
    if (result == -1)
        printf("Failed on finding ticks: %s\n", returnVCString(n)); 
    if (result != 1)
        printf("Tick value did not increment: %s\n", returnVCString(n));
    
    tick(&n, "a");

    int result1 = findTicks(n, "a");
    int result2 = findTicks(n, "b");

    if (result1 == -1 || result2 == -1 )
        printf("Failed on finding ticks: %s\n", returnVCString(n)); 

    if (result1 != 2 || result2 != 1) {
        printf("Tick value did not increment: %s\n", returnVCString(n));
    }
}
void testCopy() {
    struct vectorClock *n = NULL;
    set(&n, "a", 4);
    set(&n, "b", 1);
    set(&n, "c", 3);
    set(&n, "d", 2);
    struct vectorClock *nc = copy(n);

    int resultan = findTicks(n, "a");
    int resultbn = findTicks(n, "b");
    int resultcn = findTicks(n, "c");
    int resultdn = findTicks(n, "d");

    int resultanc = findTicks(n, "a");
    int resultbnc = findTicks(n, "b");
    int resultcnc = findTicks(n, "c");
    int resultdnc = findTicks(n, "d");

    if (resultan != resultanc || resultbn != resultbnc || resultcn != resultcnc || resultdn != resultdnc) {
        printf("Copy not the same as the original new = %s , old = %s\n",
         returnVCString(n), returnVCString(nc));
    }
}
void testMerge() {

    struct vectorClock *n1 = NULL;
    struct vectorClock *n2 = NULL;

    set(&n1, "b", 1);
    set(&n1, "a", 2);
    
    set(&n2, "b", 3);
    set(&n2, "c", 1);


    struct vectorClock *n3 = copy(n1);
    merge(n3, n2);

    int result1n = findTicks(n3, "a");
    int result2n = findTicks(n3, "b");
    int result3n = findTicks(n3, "c");

    if (result1n != 2 || result2n != 3 || result3n != 1 ){
         printf("Merge not as expected = %s , old = %s, %s\n",
            returnVCString(n3), returnVCString(n1), returnVCString(n2));
    } else {
        printf("new = %s , old = %s, %s\n",
            returnVCString(n3), returnVCString(n1), returnVCString(n2));
    }
}

int main() {
    testBasicInit();
    testCopy();
    testMerge();
}


