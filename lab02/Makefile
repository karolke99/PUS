all: ipaddr hostname2ip client4 server6 client6 server4 client46

clean:
	@rm -rf \
	01/ipaddr \
	02/hostname2ip \
	03/server6 03/client4 \
	04/server4 04/client6 \
	05/client46

ipaddr:
	gcc -o 01/ipaddr 01/ipaddr.c

hostname2ip:
	gcc -o 02/hostname2ip 02/hostname2ip.c

client4:
	g++ -o 03/client4 03/client4.cpp

server6:
	g++ -o 03/server6 03/server6.cpp

client6:
	g++ -o 04/client6 04/client6.cpp

server4:
	g++ -o 04/server4 04/server4.cpp

client46:
	g++ -o 05/client46 05/client46.cpp