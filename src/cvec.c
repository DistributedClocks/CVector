/**
 *The MIT License (MIT)
 *
 *Copyright (c) 2017-2018 University of British Columbia
 *
 *Permission is hereby granted, free of charge, to any person obtaining a copy
 *of this software and associated documentation files (the "Software"), to deal
 *in the Software without restriction, including without limitation the rights
 *to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 *copies of the Software, and to permit persons to whom the Software is
 *furnished to do so, subject to the following conditions:
 *
 *The above copyright notice and this permission notice shall be included in all
 *copies or substantial portions of the Software.
 *
 *THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 *OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 *SOFTWARE.
 *
 */

#include "cvec.h"
void mergeRemoteClock(struct vcLog *vcInfo, struct vectorClock *remoteClock);
struct vectorClock *updateClock(struct vcLog *vcInfo, char *logMsg);

struct vcLog *initCVector(char *pid, char *logName) {
    char logBuf[FILE_MAX];
    FILE *vectorLog;
    mkdir(dirname(logName), 0777);
    snprintf(logBuf, FILE_MAX, "%s-shiviz.txt", logName);

    /*Test if is is possible to use the specified logfile */
    vectorLog = fopen(logBuf, "w+");
    if (vectorLog == NULL){
        perror("ERROR: Could not open log file.");
        return NULL;
    }
    fclose(vectorLog);

    /*Create and initialize a vector clock information structure */
    struct vcLog *vcInfo = (struct vcLog *) malloc(sizeof(struct vcLog));
    strcpy(vcInfo->pid, pid); // store the provided process id
    strncpy(vcInfo->logName, logBuf, FILE_MAX); // save the log name
    vcInfo->printonscreen = 1;
    vcInfo->logging = 1; // enable logging
    pthread_mutex_init(&vcInfo->mutex, NULL);

    /*Initialize the clock and increase the time once */
    struct vectorClock *vc = clock_init(vcInfo->pid);
    tick(&vc, vcInfo->pid);
    vcInfo->vc = vc;
    if (writeLogMsg(vcInfo, "Initialization Complete") == EXIT_FAILURE) {
        perror("ERROR: Failed to log message.\n");
    }
    return vcInfo;
}

void enableLogging(struct vcLog *vcInfo) {
    vcInfo->logging = 1;
}

void disableLogging(struct vcLog *vcInfo) {
    vcInfo->logging = 0;
}

int writeLogMsg(struct vcLog *vcInfo, char*logMsg) {

    if (!vcInfo->logging) return EXIT_SUCCESS;
    /*Open in append mode */
    FILE *vectorLog = fopen(vcInfo->logName, "a+");
    if (vectorLog == NULL){
        perror("ERROR: Could not open log file.");
        return EXIT_FAILURE;
    }
    char *vcString = return_vc_string(vcInfo->vc);
    /*Calculate the length of the log message that is being written. */
    char logMessage[VC_ID_LENGTH + strlen(logMsg) + strlen(vcString) + 3];
    sprintf(logMessage, "%s %s\n%s\n", vcInfo->pid, vcString, logMsg);
    fputs(logMessage, vectorLog);
    fclose(vectorLog);
    return EXIT_SUCCESS;
}

int logLocalEvent(struct vcLog *vcInfo, char *logMsg) {
    /*Lock the global mutex */
    pthread_mutex_lock(&vcInfo->mutex);

    updateClock(vcInfo, logMsg);
    /*Free the global mutex */
    pthread_mutex_unlock(&vcInfo->mutex);
    return EXIT_SUCCESS;
}

char *prepareSend(struct vcLog *vcInfo, char *logMsg, char *packetContent, int packetLength, int *encodeLen) {
    /*Lock the global mutex */
    pthread_mutex_lock(&vcInfo->mutex);
    struct vectorClock *vc = updateClock(vcInfo, logMsg);

    /*Encode the message and append the current vector clock */
    char *data;
    size_t size;
    mpack_writer_t writer;
    mpack_writer_init_growable(&writer, &data, &size);
    mpack_write_cstr(&writer, vcInfo->pid);                 // encode the clock id as str
    mpack_write_bin(&writer, packetContent, packetLength);  // encode the message as binary

    mpack_start_map(&writer, HASH_COUNT(vc));               // encode the vector clock as map
    struct vectorClock *clock;
    for(clock=vc; clock != NULL; clock=(struct vectorClock*)(clock->hh.next)) {
        mpack_write_cstr(&writer, clock->id);               // encode the clock id as str
        mpack_write_u64(&writer, clock->time);              // encode the clock time as uin64
    }
    mpack_finish_map(&writer);

    if (mpack_writer_destroy(&writer) != mpack_ok) {
        fprintf(stderr, "An error occurred encoding the data!\n");
    return NULL;
    }
    /*Save the length of the encoded buffer in encodeLen */
    *encodeLen = writer.size;
    /*Free the global mutex */
    pthread_mutex_unlock(&vcInfo->mutex);
    return (char *) data;
}

char *unpackReceive(struct vcLog *vcInfo,  char *logMsg, char *encodedBuffer, int bufLen) {
    /*Lock the global mutex */
    pthread_mutex_lock(&vcInfo->mutex);
    mpack_reader_t reader;
    mpack_reader_init_data(&reader, encodedBuffer, bufLen);

    if (mpack_reader_error(&reader) != mpack_ok) {
        perror("ERROR: Encoded buffer content corrupted.\n");
        return NULL;
    }
    /*Decode the message*/
    char src_pid[VC_ID_LENGTH];
    char c_pid[VC_ID_LENGTH];
    mpack_expect_str_buf(&reader, src_pid, VC_ID_LENGTH);   // decode the source clock id
//    int msgSize = mpack_expect_str(&reader);                // get the message length
    int msgSize = mpack_expect_bin(&reader);                    // get the message length
    char *msg = malloc(msgSize);                                // allocate a message buffer
    mpack_read_bytes(&reader, msg, (msgSize));                  // extract the message
    //msg[msgSize] = '\0';                                      // terminate the string

    /*Construct the attached clock */
    struct vectorClock *vc = NULL;
    for (int i = mpack_expect_map(&reader); i>0; i--) {
        mpack_expect_cstr(&reader, c_pid, VC_ID_LENGTH);    // decode the source clock id
        uint64_t time = mpack_expect_u64(&reader);          // decode the source clock time
        set(&vc, c_pid, time);
    }
    mpack_done_map(&reader);

    /*Increment the local clock and merge with extracted clock*/
    tick(&vcInfo->vc, vcInfo->pid);
    mergeRemoteClock(vcInfo, vc);
    if (writeLogMsg(vcInfo, logMsg) == EXIT_FAILURE) {
        perror("ERROR: Failed to log message.\n");
    }
    /*Free the global mutex */
    pthread_mutex_unlock(&vcInfo->mutex);
    return msg;
}

void mergeRemoteClock(struct vcLog *vcInfo, struct vectorClock *remoteClock) {
    int time = find_ticks(vcInfo->vc, vcInfo->pid);
    if (time == -1) {
        perror("ERROR: Could not find process id in its vector clock.");
        return;
    }
    merge(vcInfo->vc,remoteClock);
}

struct vectorClock *updateClock(struct vcLog *vcInfo, char *logMsg) {
    struct vectorClock *vc = vcInfo->vc;
    if (vc == NULL) {
        perror("ERROR: Vector clock not initialized.");
    }
    int64_t time = find_ticks(vc, vcInfo->pid);
    if (time == -1) {
        perror("ERROR: Could not find process id in its vector clock.");
    }
    /*Increment the vector clock */
    tick(&vc, vcInfo->pid);

    if (writeLogMsg(vcInfo, logMsg) == EXIT_FAILURE) {
        perror("ERROR: Failed to log message.\n");
    }
    return vc;
}