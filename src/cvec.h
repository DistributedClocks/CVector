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

#include "./vclock/vclock.h"
#include "./mpack/mpack.h"
#include "./mpack/mpack-config.h"
#include <sys/stat.h>
#include <libgen.h>
#include <pthread.h>

#define FILE_MAX 256
//This is the Global Variable Struct that holds all the info needed to be maintained
struct goLog {
    char pid[VC_ID_LENGTH];
    struct vectorClock *vc;
    int printonscreen;
    int realtime;
    int debugmode;
    int logging;
    char logName[FILE_MAX];
};

struct goLog *initialize(char * pid, char * logName);
char *prepareSend(struct goLog *gl, char * logMsg, char* packetContent, int * encodeLen);
char *unpackReceive(struct goLog *gl,  char * logMsg, char * encodedBuffer, int bufLen);
int writeLogMsg(struct goLog *gl, char* msg);