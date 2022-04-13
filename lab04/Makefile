all: client1 server1 server2 client2 server3 client3

clean:
	@rm -rf \
	01/client1 \
	01/server1 \
	02/server2 \
	02/client2 \
	03/server3 \
	03/client3

client1:
	gcc -o 01/client1 01/client1.c

server1:
	gcc -o 01/server1 01/server1.c

server2:
	gcc -o 02/server2 02/server2.c -lsctp -Wall

client2:
	gcc -o 02/client2 02/client2.c -lsctp -Wall

server3:
	gcc -o 03/server3 03/server3.c -lsctp -Wall

client3:
	gcc -o 03/client3 03/client3.c -lsctp -Wall

