#include "govec.h"




void initialize(char * pid, char * logName) {

    struct goLog *gv = (struct goLog*) malloc(sizeof(struct goLog));
    strcpy(gv->pid, pid);
        //# These are bools that can be changed to change debuging nature of library
    gv->printonscreen = 1; //(ShouldYouSeeLoggingOnScreen)
    gv->debugmode = 0;    // (Debug)
    gv->logging = 1;

    struct vectorClock *vc1 = clockInit(gv->pid);
}