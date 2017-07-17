#include "../cvec.h"
/*
 * Print reverse byte buffer including specified length
 */

int main() {

    struct goLog *gl1 = initialize("client","mylogfile");
    char hello[100];
    strcpy(hello, "MYMSG");
    struct goLog *gl2 = initialize("testingClock","mylogbile");
    tick(&gl2->vc, "testingClock");
    tick(&gl2->vc, "testingClock");
    tick(&gl2->vc, "testingClock");
    tick(&gl2->vc, "testingClock");
    tick(&gl2->vc, "testingClock");

    printf("Encoding...\n");
    printf("Message: %s\n",hello );
    int size;
    char  * result  = prepareSend(gl2, hello, "This is going to be written to file.", &size);
    printf("%s\n",result );
    tick(&gl1->vc, gl1->pid);
    tick(&gl1->vc, "brackabracka");
    printf("Clock 1...\n");
    printVC(gl1->vc);
    char * msg = unpackReceive(gl1, result,"This has been unpacked from file.", 150);
    printf("After decoding...\n");
    printf("Message: %s\n", msg );
    printVC(gl1->vc);

/*    struct goLog *gl = initialize("client","clientlogfile");
    int i;
    for (i = 0; i < 5; i++) {
        char msg[10];
        sprintf(msg, "%d", i);
        char * inBuf[556];
        char * result  = prepareSend(gl, msg, sizeof(msg));
        char * endGame = unpackReceive(gl, result, 86);
        printf("After decoding...\n");
        printf("Message: %s\n", msg );
        printVC(gl->vc);
    }
*/
}