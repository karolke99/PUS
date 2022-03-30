all: ssrr udp tcp udpv6 ping

clean:
	@rm -rf \
	01/ssrr \
	02/udp \
	03/tcp \
	04/udpv6 \
	05/ping

ssrr:
	gcc -o 01/ssrr 01/ssrr.c

udp:
	gcc -o 02/udp 02/udp.c

tcp:
	gcc -o 03/tcp 03/tcp.c

udpv6:
	gcc -o 04/udpv6 04/udpv6.c

ping: 
	gcc -o 05/ping 05/ping.c
