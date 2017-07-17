/*
** listener.c -- a datagram sockets "server" demo
*/
#include "../src/cvec.h"

#include <unistd.h>
#include <arpa/inet.h>
#include <netdb.h>

#define SERVERPORT  "8080"    // the server port will be connecting to
#define CLIENTPORT  "8081"    // the server port will be connecting to
#define SERVERIP    "127.0.0.1"
#define MAXBUFLEN 100
#define MESSAGES 10




int initListener (char * hostPort) {
    
    int rv;
    int sockfd;
    struct addrinfo hints;
    struct addrinfo *servinfo, *p;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET; // set to AF_INET to force IPv4
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_flags = AI_PASSIVE; // use my IP

    if ((rv = getaddrinfo(NULL, hostPort, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }

    // loop through all the results and bind to the first we can
    for(p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype,
                p->ai_protocol)) == -1) {
            perror("socket");
            continue;
        }

        if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd);
            perror("bind");
            continue;
        }

        break;
    }

    if (p == NULL) {
        fprintf(stderr, "failed to bind socket\n");
        return 2;
    }
    freeaddrinfo(servinfo);
    return sockfd;
}

int server()
{
    char buf[MAXBUFLEN];
    int size;
    struct sockaddr_in their_addr;
    socklen_t addr_len = sizeof(struct sockaddr_in);
    int sockfd = initListener(SERVERPORT);
    struct goLog *gl = initialize("server","serverlogfile");

    int i;
    int n = 0, nMinOne = 0, nMinTwo= 0;
    char msg[10];
    for (i = 0; i < MESSAGES; i++) {
        int numbytes = recvfrom(sockfd, buf, MAXBUFLEN-1 , 0, (struct sockaddr *)&their_addr, &addr_len);
        memset(msg,0,10);
        strncpy(msg, unpackReceive(gl, "Received message from client.", buf, numbytes), 10);
        printf("Received message from client: ");
        printVC(gl->vc);
        if (strncmp(msg,"0",1) == 0){
                nMinTwo = 0;
                n = 0;
        } else if (strncmp(msg,"1",1) == 0){
                nMinOne = 0;
                n = 1;
        } else {
            nMinTwo = nMinOne;
            nMinOne = n;
            n = nMinOne + nMinTwo;
        }
        snprintf(msg, 10, "%d", n);
        char * inBuf = prepareSend(gl, "Replying to client.", msg, &size);
        printf("Sending message to client\n");
        sendto(sockfd, inBuf, size, 0, (struct sockaddr *)&their_addr, addr_len);
    }

    close(sockfd);
    return EXIT_SUCCESS;
}

int client()
{
    char buf[MAXBUFLEN];
    int size;

    struct sockaddr_in their_addr;
    socklen_t addr_len = sizeof(struct sockaddr_in);
    their_addr.sin_family = AF_INET;
    their_addr.sin_port = htons(atoi(SERVERPORT));
    inet_pton(AF_INET, SERVERIP, &their_addr.sin_addr);
    
    int sockfd = initListener(CLIENTPORT);
    
    struct goLog *gl = initialize("client","clientlogfile");

    int i;
    char msg[10];
    for (i = 0; i < MESSAGES; i++) {
        snprintf(msg, 10, "%d", i);
        char * inBuf = prepareSend(gl, "Sending message to server.", msg, &size);
        printf("Sending message to server\n");
        int numbytes = sendto(sockfd, inBuf, size, 0, (struct sockaddr *)&their_addr, addr_len);
        addr_len = sizeof their_addr;
        if ((numbytes = recvfrom(sockfd, buf, MAXBUFLEN-1 , 0,
            (struct sockaddr *)&their_addr, &addr_len)) == -1) {
            perror("recvfrom");
            exit(1);
        }
        strncpy(msg, unpackReceive(gl, "Received message from server.", buf, numbytes), 10);
        printf("Received message from server:\n");
        printVC(gl->vc);
        printf("Got back: %s\n", msg);
    }

    close(sockfd);
    return EXIT_SUCCESS;
}

int main (){

    system("fuser -n tcp -k 8080");
    system("fuser -n tcp -k 8081");

    int pid=fork();
    if (pid==0){
        sleep(2);
        client();    
    }
    else{ 
        server();
    }
}