all: client1 server1 server2 client2

clean:
	@rm -rf \
	01/client1 \
	01/server1 \
	02/server2 \
	02/client2

client1:
	gcc -o 01/client1 01/client1.c

server1:
	gcc -o 01/server1 01/server1.c

server2:
	gcc -o 02/server2 02/server2.c -lsctp

client2:
	gcc -o 02/client2 02/client2.c -lsctp -Wall

