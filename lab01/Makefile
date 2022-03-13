all: client1 server1 client2 server2 client3 server3 server4 

clean:
	@rm -rf \
	01/client1 01/server1 \
	02/client2 02/server2 \
	03/client3 03/server3 \
	04/server4 

client1:
	gcc 01/client1.c -o 01/client1
server1:
	gcc 01/server1.c -o 01/server1

client2:
	gcc 02/client2.c -o 02/client2
server2:
	gcc 02/server2.c -o 02/server2

client3:
	gcc -o 03/client3 03/client3.c
server3:
	gcc -o 03/server3 03/server3.c 03/libpalindrome.c

server4:
	g++ -o 04/server4 04/server4.cpp