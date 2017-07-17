/**
 * The MIT License (MIT)
 * 
 * Copyright (c) 2017-2018 University of British Columbia
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to 
 * deal in the Software without restriction, including without limitation the 
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or 
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in 
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING 
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER 
 * DEALINGS IN THE SOFTWARE.
 * 
 */

#include "./vclock/vclock.h"
#include "./mpack/mpack.h"
#include "./mpack/mpack-config.h"
#include <sys/stat.h>
#include <libgen.h>
#include <pthread.h>

/* Maximum file length of a CVector log */
#define FILE_MAX 256

/**
 * This is the basic vcLog structure used in any CVector application.
 * It contains the thread-local vector clock and process as well as 
 * information about the logging procedure and file name.
 */
struct vcLog {
    char pid[VC_ID_LENGTH]; // Fixed to VC_ID_LENGTH defined in vclock.h.
    struct vectorClock *vc; // The local vector clock map.
    char logName[FILE_MAX]; // Name of the CVector log file to write to.
    int printonscreen;      // Print all logging operations to screen.
    int logging;            // Enable logging.
};

/**
 * Initialises and returns a new vcLog structure. This vcLog structure 
 * contains the configuration of the current vector thread as well as the 
 * vector clock map and process id.
 * This structure is the basis of any further operation in CVector.
 * Any log files with the same name as "logName" will be overwritten. "pid" 
 * should be unique in the current distributed system.
 * @param pid The process id that the vector map is initialised with.
 * @param logName The name of the log file this program should write to.
 */
struct vcLog *initialize(char * pid, char * logName);

/**
 * Appends a message in the log file defined in the vcLog vcInfo structure.
 * @param vcInfo The vcLog configuration structure containing the vector clock.
 * @param logMsg Custom message will be written to the "vcInfo" log.
 */
int writeLogMsg(struct vcLog *vcInfo, char* logMsg);

/**
 * Records a local event and increments the vector clock contained in "vcInfo".
 * Also appends a message in the log file defined in the vcInfo structure.
 * @param vcInfo The vcLog configuration structure containing the vector clock.
 * @param logMsg Custom message will be written to the "vcInfo" log.
 */
int lovcInfoocalEvent(struct vcLog *vcInfo, char * logMsg);

/**
 * Encodes a buffer into a custom CVector data structure.
 * The function increments the vector clock contained in "vcInfo", appends it to 
 * the "packetContent" and converts the full message into MessagePack format.
 * The resulting length of the encoded package is stored in "encodeLen".
 * In addition, prepareSend writes a custom defined message "logMsg" to the 
 * main CVector log.
 * @param vcInfo The vcLog configuration structure containing the vector clock.
 * @param logMsg Custom message will be written to the "vcInfo" log.
 * @param packetContent The actual content of the packet we want to send out.
 * @param encodeLen A storage integer for the final encoded packet length.
 */

char *prepareSend(struct vcLog *vcInfo, char * logMsg, char* packetContent, int * encodeLen);
/**
 * Decodes a GoVector buffer, updates the local vector clock, and returns the 
 * decoded data.
 * This function takes a MessagePack buffer and extracts the vector clock as 
 * well as data. It increments the local vector clock, merges the unpacked 
 * clock with its own and returns a character representation of the data.
 * In addition, prepareSend writes a custom defined message to the main 
 * CVector log.
 * @param vcInfo The vcLog configuration structure containing the vector clock.
 * @param logMsg Custom message will be written to the "vcInfo" log.
 * @param encodedBuffer The buffer to be decoded.
 * @param bufLen Length of the buffer to initialise the MessagPack reader.
 */
char *unpackReceive(struct vcLog *vcInfo,  char * logMsg, char * encodedBuffer, int bufLen);