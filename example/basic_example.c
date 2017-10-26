#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "../src/cvec.h"

int main (){

    struct vcLog *vcInfo = init_cvector("MyProcess","basiclog");
    // Prepare  and send a Message
    int size;
    int msgLen; //The length of the message we unpacked, we do not need this here.
    char sendingMessage[50];
    strcpy(sendingMessage, "ExampleMessage");
    printf("We are packing this message: %s\n", sendingMessage);
    char * resultBuffer = prepare_send(vcInfo, "Sending Message", sendingMessage, 50, &size);
    // Unpack the message again
    char * receivedMessage = unpack_receive(vcInfo, "Receiving Message", resultBuffer, size, &msgLen);
    printf("We received this message: %s\n", receivedMessage);

    // Can be called at any point 
    log_local_event(vcInfo, "Example Complete");
    // No further events will be written to log file
    disable_logging(vcInfo);
    log_local_event(vcInfo, "This will not be logged.");
}