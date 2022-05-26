/*
** client.c -- a stream socket client demo
We took the customer's code from the link attached to the task.
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>

#include <arpa/inet.h>

#define PORT "3490" // the port client will be connecting to 

#define MAXDATASIZE 100 // max number of bytes we can get at once 

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int main(int argc, char *argv[])
{
    int sockfd, numbytes;  
    char buf[MAXDATASIZE];
    struct addrinfo hints, *servinfo, *p;
    int rv;
    char s[INET6_ADDRSTRLEN];

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    if ((rv = getaddrinfo(argv[1], PORT, &hints, &servinfo)) != 0) {
        
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }

    // loop through all the results and connect to the first we can
    for(p = servinfo; p != NULL; p = p->ai_next) {
        
        if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
            perror("client: socket");
            continue;
        }

        if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd);
            perror("client: connect");
            continue;
        }

        break;
    }

    if (p == NULL) {
        fprintf(stderr, "client: failed to connect\n");
        return 2;
    }

    inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr), s, sizeof s);
    printf("client: connecting to %s\n", s);

    freeaddrinfo(servinfo); // all done with this structure

    char input[1024];
    char push_input[1024];
    char output[1024];
    int size;

    while(1){

        printf("\nEnter command: ");
        scanf("%s", input); 
        // scanf("%[^\n^\t]", input);

        // a) PUSH:
        if(strcmp(input, "PUSH") == 0) {

            // getchar();
            scanf("%[^\n^\t]", push_input);

            strcat(input, push_input);

            // printf("push client : %s", input + 5);

            if(send(sockfd, input, sizeof(input), 0) == -1){
                
                perror("PUSH");
            }
        }

        // b) POP:
        else if(strcmp(input, "POP") == 0) {

            if(send(sockfd, "POP", 3, 0) == -1){
                
                perror("POP");
            }
        }

        // c) TOP:
        else if(strcmp(input, "TOP") == 0) {

            if(send(sockfd, "TOP", 3, 0) == -1){
                
                perror("TOP");
            }

            memset(output, 0, sizeof(output));

            size = recv(sockfd, output, 1024, 0);

            if (size == -1){

                perror("TOP");
                exit(1);
            }

            output[size] = '\0';

            printf("OUTPUT: %s\n",output);

            memset(output, 0, sizeof(output));
        }

        // d) DISPLAY FOR TESTING:
        else if(strcmp(input, "DISPLAY") == 0) {

            if(send(sockfd, "DISPLAY", 7, 0) == -1){
                
                perror("DISPLAY");
            }
        }

        else {

            printf("Invalid");
        }

        memset(input, 0, sizeof(input));
             
    }

    close(sockfd);

    return 0;
}