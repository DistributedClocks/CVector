#include "cvec.h"



int printNBytes(char * receiveBuffer, int num) {
    int i;
    for (i =0; i<=num-1; i++) {
        printf("%02x", (unsigned char) receiveBuffer[i]);
    }
    printf("\n");
    return i;
}
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

void mergeRemoteClock(struct goLog *gl, struct vectorClock * remoteClock) {
    merge(gl->vc,remoteClock);
}
/*
char *prepareSend(struct goLog *gl, char * msg, int msgLen) {

    struct vectorClock *vc = gl->vc;
    if (vc == NULL) {
        perror("ERROR: Vector clock not initialized.");
        return NULL;
    }
    int time = findTicks(vc, gl->pid);
    if (time == -1) {
        perror("ERROR: Could not find process id in its vector clock.");
        return NULL;

    }

    int num_clocks = HASH_COUNT(vc);
    int encodeLen = VC_ID_LENGTH + CLOCKSIZE * num_clocks + msgLen;
    char * encodedMsg = malloc(encodeLen);
    tick(&vc, gl->pid);
    cw_pack_context * encodedBuffer = (struct cw_pack_context*)malloc(sizeof(struct cw_pack_context));
    cw_pack_context_init (encodedBuffer, encodedMsg, encodeLen, 0);
    cw_pack_map_size (encodedBuffer, num_clocks + 2);
    cw_pack_str (encodedBuffer, gl->pid, VC_ID_LENGTH);
    cw_pack_str (encodedBuffer, msg, msgLen);


    struct vectorClock *clock;
    for(clock=vc; clock != NULL; clock=(struct vectorClock*)(clock->hh.next)) {
        cw_pack_str (encodedBuffer, clock->id, strlen(clock->id)+1);
        cw_pack_unsigned (encodedBuffer, clock->time);
    }
    return (char*) encodedBuffer;
}

char *unpackReceive(struct goLog *gl, char * encodedBuffer, int bufLen) {
    // init unpacking
    cw_unpack_context uc;
    cw_unpack_context_init (&uc, ((cw_pack_context *) encodedBuffer)->start, bufLen,  0);
    struct vectorClock *vc = NULL;
    char pid[VC_ID_LENGTH];
    char * msg = malloc(bufLen);
    // get the total map size
    cw_unpack_next(&uc);
    int i = uc.item.as.map.size;
    // get the process id
    cw_unpack_next(&uc);
    // get the encoded message
    cw_unpack_next(&uc);
    strncpy(msg, uc.item.as.str.start, bufLen);
    // reassemble the vector clock
    for (; i > 0; i--) {
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
    mergeRemoteClock(gl, vc);
    return msg;
}
*/

char *prepareSend(struct goLog *gl, char * msg, int *encodeLen) {

    struct vectorClock *vc = gl->vc;
    if (vc == NULL) {
        perror("ERROR: Vector clock not initialized.");
        return NULL;
    }
    int time = findTicks(vc, gl->pid);
    if (time == -1) {
        perror("ERROR: Could not find process id in its vector clock.");
        return NULL;

    }

    int num_clocks = HASH_COUNT(vc);
    tick(&vc, gl->pid);
    // encode to memory buffer
    char* data;
    size_t size;
    mpack_writer_t writer;
    mpack_writer_init_growable(&writer, &data, &size);
    mpack_write_cstr(&writer, gl->pid);
    mpack_write_cstr(&writer, msg);

    mpack_start_map(&writer, num_clocks);
    struct vectorClock *clock;
    for(clock=vc; clock != NULL; clock=(struct vectorClock*)(clock->hh.next)) {
        mpack_write_cstr(&writer, clock->id);
        mpack_write_u64(&writer, clock->time);
    }
    mpack_finish_map(&writer);
    // finish writing
    if (mpack_writer_destroy(&writer) != mpack_ok) {
        fprintf(stderr, "An error occurred encoding the data!\n");
    return NULL;
    }
    *encodeLen = writer.size;
    return (char*) data;
}

char *unpackReceive(struct goLog *gl, char * encodedBuffer, int bufLen) {

    mpack_reader_t reader;
    mpack_reader_init_data(&reader, encodedBuffer, bufLen);

    if (mpack_reader_error(&reader) != mpack_ok) {
        perror("ERROR: Encoded buffer content corrupted.\n");
        return NULL;
    }
    char src_pid[VC_ID_LENGTH];
    char c_pid[VC_ID_LENGTH];
    mpack_expect_str_buf(&reader, src_pid, VC_ID_LENGTH);
    int msgSize = mpack_expect_str(&reader);
    char *  msg = malloc(msgSize);
    mpack_read_bytes(&reader, msg, msgSize);
    msg[msgSize] = '\0';

    struct vectorClock *vc = NULL;
    for (int i = mpack_expect_map(&reader); i>0; i--) {
        mpack_expect_cstr(&reader, c_pid, VC_ID_LENGTH);
        uint64_t time = mpack_expect_u64(&reader);
        set(&vc, c_pid, time);
    }
    mpack_done_map(&reader);
/*    printf("Decoded PID: %s\n",src_pid );
    printf("Decoded Message: %s\n",msg );
    printf("Decoded Clock: ");
    printVC(vc);*/
    mergeRemoteClock(gl, vc);
    return msg;
}