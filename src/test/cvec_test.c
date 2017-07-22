#include "../cvec.h"
/*
 * Print reverse byte buffer including specified length
 */

int main() {

    struct vcLog *vcInfo1 = initialize("client","mylogfile");
    char hello[100];
    strcpy(hello, "MYMSG");
    struct vcLog *vcInfo2 = initialize("testingClock","mylogbile");

    printf("Encoding...\n");
    printf("Message: %s\n",hello );
    int size;
    printf("Starting encoding.\n");
    char  * result  = prepareSend(vcInfo2, "This is going to be written to file.", hello,  &size);
/*    printf("%s\n",result );
*/    tick(&vcInfo1->vc, vcInfo1->pid);
    tick(&vcInfo1->vc, "brackabracka");
    printf("Clock 1...\n");
    printVC(vcInfo1->vc);
    char * msg = unpackReceive(vcInfo1, result,"This has been unpacked from file.", 150);
    printf("After decoding...\n");
    printf("Message: %s\n", msg );
    printVC(vcInfo1->vc);

/*    struct vcLog *vcInfo = initialize("client","clientlogfile");
    int i;
    for (i = 0; i < 5; i++) {
        char msg[10];
        sprintf(msg, "%d", i);
        char * inBuf[556];
        char * result  = prepareSend(vcInfo, msg, sizeof(msg));
        char * endGame = unpackReceive(vcInfo, result, 86);
        printf("After decoding...\n");
        printf("Message: %s\n", msg );
        printVC(vcInfo->vc);
    }
*/
}