/*
** listener.c -- a datagram sockets "server" demo
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/wait.h>
#include <signal.h>
#include "../cvec/cvec.h"
#define SERVERPORT "8080"    // the server port will be connecting to
#define CLIENTPORT "8081"    // the server port will be connecting to
#define MAXBUFLEN 100
#define MESSAGES 10
int server_pid = -1;

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int server(void)
{
    int sockfd;
    struct addrinfo hints, *servinfo, *p;
    int rv;
    int numbytes;
    struct sockaddr_storage their_addr;
    char buf[MAXBUFLEN];
    socklen_t addr_len;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC; // set to AF_INET to force IPv4
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_flags = AI_PASSIVE; // use my IP

    if ((rv = getaddrinfo(NULL, SERVERPORT, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }

    // loop through all the results and bind to the first we can
    for(p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype,
                p->ai_protocol)) == -1) {
            perror("listener: socket");
            continue;
        }

        if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd);
            perror("listener: bind");
            continue;
        }

        break;
    }

    if (p == NULL) {
        fprintf(stderr, "listener: failed to bind socket\n");
        return 2;
    }

    struct goLog *gl = initialize("server","serverlogfile");

    int i;
    int n = 0, nMinOne = 0, nMinTwo= 0;
    char msg[10];
    for (i = 0; i < MESSAGES; i++) {
        int size;
        addr_len = sizeof their_addr;
        if ((numbytes = recvfrom(sockfd, buf, MAXBUFLEN-1 , 0,
            (struct sockaddr *)&their_addr, &addr_len)) == -1) {
            perror("recvfrom");
            exit(1);
        }
        memset(msg,0,10);
        strncpy(msg, unpackReceive(gl, "Received message from client.", buf, numbytes), 10);
        printf("Received message from server:\n");
        printVC(gl->vc);
        printf("Received: %s\n", msg);
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
        memset(msg,0,10);

        sprintf(msg, "%d", n);
        char * inBuf = prepareSend(gl, "Replying to client.", msg, &size);
        printf("Sending message to client\n");
        if ((numbytes = sendto(sockfd, inBuf, size, 0,
                 (struct sockaddr *)&their_addr, addr_len)) == -1) {
            perror("talker: sendto");
            exit(1);
        }
        sleep(1);
    }
    freeaddrinfo(servinfo);
    close(sockfd);

    return 0;
}

int client()
{

    int sockfd;
    struct addrinfo hints, *servinfo, *clientinfo, *p;
    int rv;
    int numbytes;
    struct sockaddr_storage their_addr;
    char buf[MAXBUFLEN];
    socklen_t addr_len;


    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC; // set to AF_INET to force IPv4
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_flags = AI_PASSIVE; // use my IP

    if ((rv = getaddrinfo(NULL, CLIENTPORT, &hints, &clientinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }

    // loop through all the results and bind to the first we can
    for(p = clientinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype,
                p->ai_protocol)) == -1) {
            perror("listener: socket");
            continue;
        }

        if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd);
            perror("listener: bind");
            continue;
        }

        break;
    }

    if (p == NULL) {
        fprintf(stderr, "listener: failed to bind socket\n");
        return 2;
    }

    struct goLog *gl = initialize("client","clientlogfile");
    if ((rv = getaddrinfo("127.0.0.1", SERVERPORT, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }

    int i;
    for (i = 0; i < MESSAGES; i++) {
        char msg[10];
        sprintf(msg, "%d", i);
        int size;
        char * inBuf = prepareSend(gl, "Sending message to server.", msg, &size);
        printf("Sending message to server\n");
        if ((numbytes = sendto(sockfd, inBuf, size, 0,
                 servinfo->ai_addr, servinfo->ai_addrlen)) == -1) {
            perror("talker: sendto");
            exit(1);
        }
        addr_len = sizeof their_addr;
        if ((numbytes = recvfrom(sockfd, buf, MAXBUFLEN-1 , 0,
            (struct sockaddr *)&their_addr, &addr_len)) == -1) {
            perror("recvfrom");
            exit(1);
        }
        memset(msg,0,10);
        strncpy(msg, unpackReceive(gl, "Received message from server.", buf, numbytes), 10);
        printf("Received message from client:\n");
        printVC(gl->vc);
        printf("Got back: %s\n", msg);
        sleep(1);
    }


    freeaddrinfo(servinfo);
    close(sockfd);

    return 0;
}

int main (){

    system("fuser -n tcp -k 8080");
/*    pid_t server_pid = fork();
*//*    if (server_pid > 0) {
        sleep(2);
        client();
        kill(0,SIGTERM);
    }
    else if (server_pid == 0){
        printf("hello");
        server();
    }*/
    int pid=fork();
    if (pid==0){
        sleep(2);
        client();
/*        kill(getppid(),SIGTERM);
*/    }
    else{ /* avoids error checking*/
        server();
    }
}