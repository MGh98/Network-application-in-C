C = gcc
CFLAGS = -Wall
CLIENT = client
SERVER = server


all: client server
	
	
client: client.c
	$(C) $(CFLAGS) client.c -o $(CLIENT)
server: server.c
	$(C) $(CFLAGS) server.c -o $(SERVER)


clean:
	rm -f client
	rm -f server



