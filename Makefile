.PHONY: all clean

all: server client

server: server.o
	gcc server.o -o server -lpthread
client: client.o 
	gcc client.o -o client
server.o: server.c 
	gcc -c server.c 
client.o: client.c 
	gcc -c client.c
clean :
	rm -f *.o client server test file.txt