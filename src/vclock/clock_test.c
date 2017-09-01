#include "../vclock/vclock.h"

void testBasicInit() {
    struct vectorClock *n = NULL;
    tick(&n, "a");
    tick(&n, "b");
    int result = find_ticks(n, "a");
    if (result == -1)
        printf("Failed on finding ticks: %s\n", return_vc_string(n)); 
    if (result != 1)
        printf("Tick value did not increment: %s\n", return_vc_string(n));
    
    tick(&n, "a");

    int result1 = find_ticks(n, "a");
    int result2 = find_ticks(n, "b");

    if (result1 == -1 || result2 == -1 )
        printf("Failed on finding ticks: %s\n", return_vc_string(n)); 

    if (result1 != 2 || result2 != 1) {
        printf("Tick value did not increment: %s\n", return_vc_string(n));
    }
}
void testCopy() {
    struct vectorClock *n = NULL;
    set(&n, "a", 4);
    set(&n, "b", 1);
    set(&n, "c", 3);
    set(&n, "d", 2);
    struct vectorClock *nc = copy(n);

    int resultan = find_ticks(n, "a");
    int resultbn = find_ticks(n, "b");
    int resultcn = find_ticks(n, "c");
    int resultdn = find_ticks(n, "d");

    int resultanc = find_ticks(n, "a");
    int resultbnc = find_ticks(n, "b");
    int resultcnc = find_ticks(n, "c");
    int resultdnc = find_ticks(n, "d");

    if (resultan != resultanc || resultbn != resultbnc || resultcn != resultcnc || resultdn != resultdnc) {
        printf("Copy not the same as the original new = %s , old = %s\n",
         return_vc_string(n), return_vc_string(nc));
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

    int result1n = find_ticks(n3, "a");
    int result2n = find_ticks(n3, "b");
    int result3n = find_ticks(n3, "c");

    if (result1n != 2 || result2n != 3 || result3n != 1 ){
         printf("Merge not as expected = %s , old = %s, %s\n",
            return_vc_string(n3), return_vc_string(n1), return_vc_string(n2));
    } else {
        printf("new = %s , old = %s, %s\n",
            return_vc_string(n3), return_vc_string(n1), return_vc_string(n2));
    }
}

int main() {
    testBasicInit();
    testCopy();
    testMerge();
}


