#include "../cvec.h"
/*
 * Print reverse byte buffer including specified length
 */

int main() {

    struct vcLog *vcInfo1 = initCVector("client","mylogfile");
    char hello[100];
    strcpy(hello, "MYMSG");
    struct vcLog *vcInfo2 = initCVector("testingClock","mylogbile");

    printf("Encoding...\n");
    printf("Message: %s\n",hello );
    int size;
    printf("Starting encoding.\n");
    char  * result  = prepareSend(vcInfo2, "This is going to be written to file.", hello,  &size);
    tick(&vcInfo1->vc, vcInfo1->pid);
    tick(&vcInfo1->vc, "Testing..");
    printf("Clock 1...\n");
    printVC(vcInfo1->vc);
    char * msg = unpackReceive(vcInfo1, result,"This has been unpacked from file.", 150);
    printf("After decoding...\n");
    printf("Message: %s\n", msg );
    printVC(vcInfo1->vc);
}