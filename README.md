In this task we implemented a synchronized stack that supports multiple processes, and several clients can connect to it in parallel. The stack is synchronized between the clients using shared memory.

The commands we executed for the stack are:

PUSH
POP
TOP
DISPLAY

The stack uses the malloc and free of memory that we implemented ourselves using mmap. 

How to run our program:

1. Open the terminal and write: make all

2. In order to run the server, write: ./server

3. In order to run the client, open a new terminal and write: ./client

How to run our test:

1. Open the terminal and write: make test

2. In order to run the test, write: ./test