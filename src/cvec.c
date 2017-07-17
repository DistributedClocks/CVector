/**
 * The MIT License (MIT)
 * 
 * Copyright (c) 2017-2018 University of British Columbia
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 * 
 */

#include "cvec.h"
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

struct vcLog *initialize(char * pid, char * logName) {
    char logBuf[FILE_MAX];
    FILE *vectorLog;
    mkdir(dirname(logName), 0777);
    snprintf(logBuf, FILE_MAX, "%s.log", logName);

    vectorLog = fopen(logBuf, "w+");
    if (vectorLog == NULL){
        perror("ERROR: Could not open log file.");
        return NULL;
    }
    struct vcLog *vcInfo = (struct vcLog*) malloc(sizeof(struct vcLog));
    strcpy(vcInfo->pid, pid);
    strncpy(vcInfo->logName, logBuf, FILE_MAX); 
    vcInfo->printonscreen = 1;
    vcInfo->logging = 1;
    struct vectorClock *vc = clockInit(vcInfo->pid);
    tick(&vc, vcInfo->pid);
    vcInfo->vc = vc;
    fprintf(vectorLog,"%s %s\nInitialization Complete\n",
         vcInfo->pid, returnVCString(vc));
    fclose(vectorLog);
    return vcInfo;
}

int writeLogMsg(struct vcLog *vcInfo, char* logMsg) {
    FILE *vectorLog = fopen(vcInfo->logName, "a+");
    if (vectorLog == NULL){
        perror("ERROR: Could not open log file.");
        return EXIT_FAILURE;
    }
    char * vcString = returnVCString(vcInfo->vc);
    char logMessage[VC_ID_LENGTH + strlen(logMsg) + strlen(vcString)];

    sprintf(logMessage, "%s %s\n%s\n", vcInfo->pid, vcString, logMsg);

    fputs(logMessage, vectorLog);
    fclose(vectorLog);
    return EXIT_SUCCESS;
}

int lovcInfoocalEvent(struct vcLog *vcInfo, char * logMsg) {
    pthread_mutex_lock(&mutex);
    struct vectorClock *vc = vcInfo->vc;
    if (vc == NULL) {
        perror("ERROR: Vector clock not initialized.");
        return EXIT_FAILURE;
    }
    int time = findTicks(vc, vcInfo->pid);
    if (time == -1) {
        perror("ERROR: Could not find process id in its vector clock.");
        return EXIT_FAILURE;
    }

    tick(&vc, vcInfo->pid);
    
    if (writeLogMsg(vcInfo, logMsg) == EXIT_FAILURE) {
        perror("ERROR: Failed to log message.\n");
        return EXIT_FAILURE;
    }
    pthread_mutex_unlock(&mutex);

    return EXIT_SUCCESS;
}

char *prepareSend(struct vcLog *vcInfo, char * logMsg, char* packetContent, int * encodeLen) {
    pthread_mutex_lock(&mutex);
    struct vectorClock *vc = vcInfo->vc;
    if (vc == NULL) {
        perror("ERROR: Vector clock not initialized.");
        return NULL;
    }
    int time = findTicks(vc, vcInfo->pid);
    if (time == -1) {
        perror("ERROR: Could not find process id in its vector clock.");
        return NULL;
    }
    tick(&vc, vcInfo->pid);
    
    if (writeLogMsg(vcInfo, logMsg) == EXIT_FAILURE) {
        perror("ERROR: Failed to log message.\n");
    }

    // encode to memory buffer
    char* data;
    size_t size;
    mpack_writer_t writer;
    mpack_writer_init_growable(&writer, &data, &size);
    mpack_write_cstr(&writer, vcInfo->pid);
    mpack_write_cstr(&writer, packetContent);

    mpack_start_map(&writer, HASH_COUNT(vc));
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
    pthread_mutex_unlock(&mutex);
    return (char*) data;
}

void mergeRemoteClock(struct vcLog *vcInfo, struct vectorClock * remoteClock) {
    int time = findTicks(vcInfo->vc, vcInfo->pid);
    if (time == -1) {
        perror("ERROR: Could not find process id in its vector clock.");
        return;
    }
    merge(vcInfo->vc,remoteClock);
}

char *unpackReceive(struct vcLog *vcInfo,  char * logMsg, char * encodedBuffer, int bufLen) {
    pthread_mutex_lock(&mutex);
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
    tick(&vcInfo->vc, vcInfo->pid);
    mergeRemoteClock(vcInfo, vc);
    if (writeLogMsg(vcInfo, logMsg) == EXIT_FAILURE) {
        perror("ERROR: Failed to log message.\n");
    }
    pthread_mutex_unlock(&mutex);
    return msg;
}
