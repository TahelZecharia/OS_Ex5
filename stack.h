/*
We used the code at the following link:
https://stackoverflow.com/questions/8475609/implementing-your-own-malloc-free-with-mmap-and-munmap
*/

// https://github.com/chrisvrose/os3-malloc-mmap/blob/master/src/mapmanagement.c

#pragma once

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

#include <sys/mman.h>
#include <fcntl.h>

// #include "malloc2.h"

void push(char* val); 
void pop();
void display();
char* top();
char* stack_handler(char* str, char* input, char* output);

// # define malloc(x) _malloc(x);
// # define free(x) _free(x);

struct Node {
   char data[1024];
   struct Node *next;
};

struct Stack {
    
    int _size;
    struct Node *_address;
};

struct Stack *stack = NULL;

#define BACKLOG 10   // how many pending connections queue will hold

struct flock locker;

// pthread_mutex_t lock;

int file;

int open_file(){

    printf(" ** open file **");
    
    file = open("file.txt", O_WRONLY | O_CREAT); 
    
    if (file == -1) {
        printf("Error");
    }

    memset(&locker, 0, sizeof(locker)); 
    
    return file;
}

void* sharedMemory() {
  // TODO: Validate size.

  printf(" ** shared memory **");

  void *add = mmap(NULL, (size_t)(sizeof(struct Node)*1000), PROT_READ | PROT_WRITE,
                   MAP_ANONYMOUS | MAP_SHARED, file, 0);
  
  assert(add != MAP_FAILED);

  // We can't use printf here because printf internally calls `malloc` and thus
  // we'll get into an infinite recursion leading to a segfault.
  // Instead, we first write the message into a string and then use the `write`
  // system call to display it on the console.
//   char buf[1024];
//   snprintf(buf, 1024, "%s:%d malloc(%zu): Allocated %zu bytes at %p\n",
//            __FILE__, __LINE__, 10000, sizeof(struct Node)*10000, address);
//   write(STDOUT_FILENO, buf, strlen(buf) + 1);

  stack->_size = 0;
  stack->_address = (struct Node*)add;

  return add;
}

void freeSharedMemory(void *ptr) {
  
  for( int i = 0; i<stack->_size; i++){
      stack->_address--;
  }
  
  munmap(stack->_address, sizeof(struct Node)*1000);
}



void *_malloc(size_t size) {
    stack->_address++;
    stack->_size++;

 return stack->_address;
}

void _free(void *ptr) {
    
    stack->_address--;
    stack->_size--; 
}

char* stack_handler(char* str, char* input, char* output){

    // pthread_mutex_lock(&lock);

    locker.l_type = F_WRLCK;    
    
    fcntl(file, F_SETLKW, &locker);

    
    char* out = NULL;

    // a) PUSH:
    if(strcmp(str, "PUSH") == 0) {

        push(input);
    }

    // b) POP:
    else if(strcmp(str, "POP") == 0) {

        pop();
    }

    // c) TOP:
    else if(strcmp(str, "TOP") == 0) {

        out = top();
    }

    // d) DISPLAY FOR TESTING:
    else if(strcmp(str, "DISPLAY") == 0) {

        display(); 
    }
         
    else {

        printf("Invalid");
    }

    // pthread_mutex_unlock(&lock);

    locker.l_type = F_UNLCK;
    
    fcntl (file, F_SETLKW, &locker);

    
    return out;
}

void push(char* val) {

    printf("*** PUSH FUNC ***\n");
    printf("The pushed element is :%s\n", val);

    struct Node* temp = stack->_address;
    
    struct Node* newnode = (struct Node*)_malloc(sizeof(struct Node));
    
    strcpy(newnode->data,val);
    
    newnode->next = temp;
}

void pop() {

    printf("*** POP FUNC ***\n");

   if(stack->_size == 0){
        printf("Error : Trying to pop from empty stack\n");
        return;
   }
   
   else {
      
      printf("The popped element is : %s\n", stack->_address->data);
      _free(stack->_address);
   }
}

void display() {

    printf("*** DISPLAY FUNC ***\n");
   
   struct Node* ptr;
   
   if(stack->_size == 0){
        printf("Stack is empty\n");
        return;
   
   } else {
      
      ptr = stack->_address;
      printf("Stack elements are:\n");
      
      for(int i = stack->_size; i > 0; i--) {
         
         printf("%s", ptr->data);
         printf(", ");
         ptr = ptr->next;
      
      }
      printf("\n");
   }
}

char* top(){

    printf("*** TOP FUNC ***\n");
   
    if(stack->_size == 0){
        
        printf("Stack is empty\n");
        return NULL;
   
    } else {
       
        // struct Node* ptr;
        // ptr = stack->_address;
        printf("The top element is: %s\n", stack->_address->data);
        return stack->_address->data;
    }
}