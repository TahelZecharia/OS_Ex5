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
#include <assert.h>
#include "stack.h"


pthread_t thread[100];

char* abc = "abcdefghijklmnopqrstuvwxyz";

int successes = 0;

void* thread_push(){

    // pthread_mutex_lock(&lock);

    char input[] = {abc++[0], '\0'};

    // pthread_mutex_unlock(&lock);

    stack_handler("PUSH", input, NULL);
    
    return NULL;

}

void* thread_pop(){

    stack_handler("POP", NULL, NULL);
}

int main(){

    // pthread_mutex_init(&lock, NULL);

    for (int i = 'a'; i < 'z' + 1; i++){
        
        pthread_create(&(thread[i]), NULL, &thread_push, NULL);
    }

    for (int i = 'a'; i < 'z' + 1; i++){
        
        pthread_join(thread[i], NULL);
    }

    char* element;
    char letter[] = {'z', '\0'};

    for (int i = 'a'; i < 'z' + 1; i++){
        
        element = stack_handler("TOP", NULL, NULL);

        stack_handler("POP", NULL, NULL);
        
        if (strcmp(letter, element) == 0){

            printf("SUCCESS! %s == %s\n", letter, element);

            successes++;

        }

        (*letter)--;
    }

    if (successes == ('z' - 'a' + 1)){

        printf("\n*********************************\n");
        printf("\n   THE TEST ENDED SUCCESSFULLY \n");
        printf("\n*********************************\n\n");
    }

    assert(successes == ('z' - 'a' + 1));

    // pthread_mutex_destroy(&lock);

    return 0;
}