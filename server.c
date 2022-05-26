/*
** server.c -- a stream socket server demo
We used the code at the following link:
https://www.geeksforgeeks.org/handling-multiple-clients-on-server-with-multithreading-using-socket-programming-in-c-cpp/
*/

#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h> 
#include <stdbool.h>
// #include "malloc.h"
#include "stack.h"


void sigchld_handler(int s);
void *get_in_addr(struct sockaddr *sa);

#define PORT "3490"  // the port users will be connecting to

#define BACKLOG 10   // how many pending connections queue will hold

pthread_t thread[100];

void* threads_handler(void* new_fd){

    int fd = *(int*)new_fd;
    // free(new_fd);
    char input[1024];
    char* output;

    while(1) {

		if(recv(fd, input, 1024, 0) > 0) {

            printf("recv: %s\n", input);
            
            // a) PUSH:
            if(strncmp(input, "PUSH", 4) == 0) {

                stack_handler("PUSH", (input + 5), NULL);               
            }

            // b) POP:
            else if(strncmp(input, "POP", 3) == 0) {

                stack_handler("POP", NULL, NULL);
            }

            // c) TOP:
            else if(strncmp(input, "TOP", 3) == 0) {

                output = stack_handler("TOP", NULL, output);

                if (output != NULL){

                    if(send(fd, output, strlen(output), 0) == -1){
                
                        perror("TOP");
                    }

                } else {
                    
                    if (send(fd, "Stack Is Empty", 14, 0) == -1){
                
                        perror("TOP");
                    }
                }
            }

            // d) DISPLAY FOR TESTING:
            else if(strncmp(input, "DISPLAY", 7) == 0) {

                stack_handler("DISPLAY", NULL, NULL);

            }
				
		} else {
        
            break;
        }

        memset(input, 0, sizeof(input));
    }
    
    return NULL;
}

int main(void){

    int sockfd, new_fd;  // listen on sock_fd, new connection on new_fd
    struct addrinfo hints, *servinfo, *p;
    struct sockaddr_storage their_addr; // connector's address information
    socklen_t sin_size;
    struct sigaction sa;
    int yes=1;
    char s[INET6_ADDRSTRLEN];
    int rv;

    file = open_file();

    stack = (struct Stack*)mmap(NULL, sizeof(struct Stack), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, -1, 0);
    stack->_address = (struct Node*)mmap(NULL, sizeof(struct Node)*1000, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, -1, 0);

    stack->_size = 0;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE; // use my IP

    if ((rv = getaddrinfo(NULL, PORT, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }

    // loop through all the results and bind to the first we can
    for(p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype,
                p->ai_protocol)) == -1) {
            perror("server: socket");
            continue;
        }

        if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
            perror("setsockopt");
            exit(1);
        }

        if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd);
            perror("server: bind");
            continue;
        }

        break;
    }

    freeaddrinfo(servinfo); // all done with this structure

    if (p == NULL)  {
        fprintf(stderr, "server: failed to bind\n");
        exit(1);
    }

    if (listen(sockfd, BACKLOG) == -1) {
        perror("listen");
        exit(1);
    }

    sa.sa_handler = sigchld_handler; // reap all dead processes
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    if (sigaction(SIGCHLD, &sa, NULL) == -1) {
        perror("sigaction");
        exit(1);
    }
    int i = 0;
    printf("server: waiting for connections...\n");

    while(1) {  // main accept() loop
        
        sin_size = sizeof their_addr;
        
        new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);
        
        if (new_fd == -1) {
            perror("accept");
            continue;
        }

        inet_ntop(their_addr.ss_family, get_in_addr((struct sockaddr *)&their_addr), s, sizeof s);
        printf("server: got connection from %s\n", s);

        // int *new_sock = malloc(sizeof (int));
        // *new_sock = new_fd;
        
        // if (pthread_create(&thread[i++], NULL,&threads_handler, &new_fd)!= 0){
        //     printf("Failed to create thread\n");
        //     close(new_fd);
        //     // free(new_sock);
        //     exit(0);
        // }

        if (!fork()){
            i++;
            close(sockfd);
            threads_handler(&new_fd);
            exit(0);
        }

        close(file);
        close(new_fd);  // parent doesn't need this
    }

    // pthread_mutex_destroy(&lock);

    return 0;
}

void sigchld_handler(int s)
{
    // waitpid() might overwrite errno, so we save and restore it:
    int saved_errno = errno;

    while(waitpid(-1, NULL, WNOHANG) > 0);

        errno = saved_errno;
}


// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}