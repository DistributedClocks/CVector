/*
** listener.c -- a datagram sockets "server" demo
*/
#include "../src/cvec.h"

#include <unistd.h>
#include <arpa/inet.h>
#include <netdb.h>

#define SERVERPORT  8080    // the server port will be connecting to
#define CLIENTPORT  8081    // the server port will be connecting to
#define SERVERIP    "127.0.0.1"
#define MAXBUFLEN 100
#define MESSAGES 10


/* Initialize a listening socket */
int init_socket(int hostPort) {
    
    int sockfd;
    struct sockaddr_in l_addr;

    /* Get a datagram socket */
    if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) {
        perror("Socket");
        exit(1);
    }

    /* Initialize the server address */
    l_addr.sin_family = AF_INET;
    l_addr.sin_port = htons(hostPort);
    l_addr.sin_addr.s_addr = INADDR_ANY;

    /* Assign the port number to the socket the server address */
    if ( bind(sockfd, (struct sockaddr *)&l_addr, sizeof(l_addr)) != 0 ) {
        perror("Failed to bind a socket.");
        exit(1);
    }
    return sockfd;
}

/* The server program */
int server() {
    char buf[MAXBUFLEN];
    int size;
    struct sockaddr_in their_addr;
    socklen_t addr_len = sizeof(struct sockaddr_in);

    /* Get the socket */
    int sockfd = init_socket(SERVERPORT);
    /* Initialize a vector clock with id server and the log serverlogfile */
    struct vcLog *vcInfo = init_cvector("server","serverlogfile");

    int i;
    int n = 0, nMinOne = 0, nMinTwo= 0;
    int msgLen = sizeof(int32_t);
    char msg[msgLen];
    for (i = 0; i < MESSAGES; i++) {
        /* Wait for a client message */
        int numbytes = recvfrom(sockfd, buf, MAXBUFLEN-1 , 0,
            (struct sockaddr *)&their_addr, &addr_len);
        memset(msg,0,msgLen);
        /* Decode the buffer, save the vector clock, and store the message. */
        memcpy(msg, unpack_receive(vcInfo, "Received message from client.", buf, numbytes, &msgLen), msgLen);
        /* Convert the integer back into host order */
        int32_t intMsg;
        memcpy(&intMsg, msg, msgLen);
        intMsg = ntohl(intMsg);
        printf("Received message from client: %d\n", intMsg);
        /* Calculate fibonacci */
        if (intMsg == 0){
            nMinTwo = 0;
            n = 0;
        } else if (intMsg == 1){
            nMinOne = 0;
            n = 1;
        } else {
            nMinTwo = nMinOne;
            nMinOne = n;
            n = nMinOne + nMinTwo;
        }
        /* Encode the message and vector clock. */
        /* We need to convert integers into network byte order to be interoperable */
        printf("Responding to client with value %d\n", n);
        int32_t byteOrderInt = htonl(n);
        char *inBuf = prepare_send(vcInfo, "Responding to client.", (char*) &byteOrderInt, msgLen, &size);
        sendto(sockfd, inBuf, size, 0, (struct sockaddr *)&their_addr, addr_len);
    }

    close(sockfd);
    return EXIT_SUCCESS;
}

/* The client program */
int client() {
    char buf[MAXBUFLEN];
    int size;

    struct sockaddr_in their_addr;
    socklen_t addr_len = sizeof(struct sockaddr_in);
    their_addr.sin_family = AF_INET;
    their_addr.sin_port = htons(SERVERPORT);
    inet_pton(AF_INET, SERVERIP, &their_addr.sin_addr);
    
    /* Get the socket */
    int sockfd = init_socket(CLIENTPORT);
    /* Initialize a vector clock with id client and the log clientlogfile */
    struct vcLog *vcInfo = init_cvector("client","clientlogfile");

    int i;
    int msgLen = sizeof(int32_t);
    char msg[msgLen];
    for (i = 0; i < MESSAGES; i++) {
        /* Encode the message and vector clock. */
        /* We need to convert integers into network byte order to be interoperable */
        int32_t byteOrderInt = htonl(i);
        char *inBuf = prepare_send(vcInfo, "Sending message to server.", (char*) &byteOrderInt, msgLen, &size);
        printf("Sending message to server\n");
        sendto(sockfd, inBuf, size, 0, (struct sockaddr *)&their_addr, addr_len);
        addr_len = sizeof their_addr;
        int numbytes = recvfrom(sockfd, buf, MAXBUFLEN-1 , 0,
            (struct sockaddr *)&their_addr, &addr_len);
        /* Decode the buffer, save the vector clock, and store the message. */
        memcpy(msg, unpack_receive(vcInfo, "Received message from server.", buf, numbytes, &msgLen), msgLen);
        /* Convert the integer back into host order */
        int32_t intMsg;
        memcpy(&intMsg, msg, msgLen);
        intMsg = ntohl(intMsg);
        printf("Received value %d from server.\n", intMsg);
    }

    close(sockfd);
    return EXIT_SUCCESS;
}

int main (){

    /* Clean processes listening on the required ports */
    //system("fuser -n tcp -k 8080");
    //system("fuser -n tcp -k 8081");

    int pid=fork();
    if (pid==0){
        sleep(2);
        client();
    }
    else{ 
        server();
    }
    /* Call a script to generate a Shiviz compatible log. */
    system("./shiviz.sh");
}