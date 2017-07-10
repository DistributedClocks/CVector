#include "govec.h"




struct goLog *initialize(char * pid, char * logName) {

    char logBuf[FILE_MAX];
    FILE *vectorLog;

    mkdir(dirname(logName), 0777);
    snprintf(logBuf, FILE_MAX, "%s.log",logName);
    
    vectorLog = fopen(logBuf, "w+");
    if (vectorLog == NULL){
        perror("ERROR: Could not open log file.");
        return NULL;
    }

    struct goLog *gl = (struct goLog*) malloc(sizeof(struct goLog));
    strcpy(gl->pid, pid);

    gl->printonscreen = 1;
    gl->debugmode = 0;
    gl->logging = 1;

    struct vectorClock *vc = clockInit(gl->pid);
    gl->vc = vc;
    fprintf(vectorLog,"Initialization Complete\n%s %s",
         gl->pid, returnVCString(vc));
    fclose(vectorLog);
    return gl;
}

int printNBytes(char * receiveBuffer, int num) {
    int i;

    // for (i = num-1; i>=0; i--) {
    //  printf("%02x", (unsigned char) receiveBuffer[i]);
    // }
    for (i =0; i<=num-1; i++) {
        printf("%02x", (unsigned char) receiveBuffer[i]);
    }
    printf("\n");
    return i;
}
cw_pack_context *prepareSend(struct goLog *gl, char * msg, char *buf) {

    struct vectorClock *vc = gl->vc;
    if (vc == NULL) {
        perror("ERROR: Vector clock not initialized.");
        return NULL;
    }
    int time = findTicks(vc, gl->pid);
    if (time == -1) {
        perror("ERROR: Could not find process id in its vector clock.");
    }
    tick(&vc, gl->pid);

/*    struct clockPayload cPayload;
    cPayload.pid = gl->pid;
    cPayload.vc = vc;
    cPayload.payload = buf;*/

    cw_pack_context * pc = (struct cw_pack_context*)malloc(sizeof(struct cw_pack_context));
    cw_pack_context_init (pc, buf, 50, 0);
    cw_pack_map_size (pc, HASH_COUNT(vc));
    int num_clocks = HASH_COUNT(vc);
    struct vectorClock *clock;
    for(clock=vc; clock != NULL; clock=(struct vectorClock*)(clock->hh.next)) {
        cw_pack_str (pc, clock->id, strlen(clock->id)+1);
        cw_pack_unsigned (pc, clock->time);
    }
    printVC(vc);
    return pc;
}

struct vectorClock *unpackReceive(cw_pack_context * pc) {
    cw_unpack_context uc;
    cw_unpack_context_init (&uc, pc->start, 50,  0);
    struct vectorClock *vc = NULL;
    char pid[50];
    cw_unpack_next(&uc);
    int i;
    for (i =uc.item.as.map.size; i > 0; i--) {
        cw_unpack_next(&uc);
        if (uc.item.type == CWP_ITEM_STR) {
            strcpy(pid, uc.item.as.str.start);
            cw_unpack_next(&uc);
            if (uc.item.type == CWP_ITEM_POSITIVE_INTEGER) {
                uint64_t time = uc.item.as.u64;
                set(&vc, pid, time);
            }
        }
    }
    return vc;
} 