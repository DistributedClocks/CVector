CVector
========

This library can be added to a C project to generate a [ShiViz](http://bestchai.bitbucket.org/shiviz/)-compatible vector-clock timestamped log of events in a concurrent or distributed system.
CVector is written in ANSI C11.

* cvec/    : Contains the Library and all its dependencies
* example/  : Contains examples which can be instrumented with CVector


### Usage

CVector has the following dependencies:

VClock - A vector clock library written in C.
src/vclock/vclock.h

MPack[https://github.com/ludocode/mpack] - A [MessagePack](http://msgpack.org/index.html) implementation.
src/mpack/mpack.h

To use CVector simply import .src/cvec.h.
You can compile your project by including the following header files:
src/vclock/cvec.h src/vclock/vclock.h src/mpack/mpack.h

Or by generating the libcvec library via this command:

```make
make lib
```
and then compiling your program by including src/libcvec.a

```make
gcc -o myApp src/libcvec.a
```

### Index
```
struct vcLog *initialize(char * pid, char * logName);
```
```
int writeLogMsg(struct vcLog *vcInfo, char* logMsg);
```
```
int loglocalEvent(struct vcLog *vcInfo, char * logMsg);
```
```
char *prepareSend(struct vcLog *vcInfo, char * logMsg, char* packetContent, int * encodeLen);
```
```
char *unpackReceive(struct vcLog *vcInfo,  char * logMsg, char * encodedBuffer, int bufLen);
```
```
void DisableLogging (struct vcLog *vcInfo)
```

###   Examples

The following is a basic example of how this library can be used:

```c
#include "../src/cvec.h"

int main (){

    struct vcLog *vcInfo = initialize("MyProcess","basiclog");
    //Prepare a Message
    int size;
    char sendingMessage[50];
    strcpy(sendingMessage, "ExampleMessage");
    printf("We are packing this message: %s\n", sendingMessage);

    char * resultBuffer = prepareSend(vcInfo, "Sending Message", sendingMessage, &size);

    char * receivedMessage = unpackReceive(vcInfo, "Receiving Message", resultBuffer, size);
    printf("We received this message: %s\n", receivedMessage);

    //Can be called at any point 
    logLocalEvent(vcInfo, "Example Complete");
    //No further events will be written to log file*/
    disableLogging(vcInfo);
    //Can be called at any point 
    logLocalEvent(vcInfo, "This will not be logged.");
}
```

This produces the log "basiclog.shiviz" :

    MyProcess {"MyProcess":1}
    Initialization Complete
    MyProcess {"MyProcess":2}
    Sending Message
    MyProcess {"MyProcess":3}
    Receiving Message
    MyProcess {"MyProcess":4}
    Example Complete


An executable example of a similar program can be found in
[Examples/client_server.c](https://github.com/DistributedClocks/CVector/blob/master/example/client_server.c)
