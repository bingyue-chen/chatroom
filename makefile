#compiler
CC = g++
#CC = g++

#cflags
CFLAGS = -g -ansi -pthread 

#object

all: b013040040_server b013040040_client

b013040040_server: global_def.o user.o usermanage.o server_socket.o server.o
	$(CC) $(CFLAGS) global_def.o user.o usermanage.o server_socket.o server.o  -o $@

b013040040_client: global_def.o client_socket.o client.o
	$(CC) $(CFLAGS) global_def.o client_socket.o client.o  -o $@

%.o: %.cpp
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm *.o 
dep:
	$(CC) -MM *.c > dep.txt

