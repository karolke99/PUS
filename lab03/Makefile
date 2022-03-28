all: ssrr udp udpv6 ping

clean:
	@rm -rf \
	01/ssrr \
	02/udp \
	04/udpv6 \
	05/ping

ssrr:
	gcc -o 01/ssrr 01/ssrr.c

udp:
	gcc -o 02/udp 02/udp.c

udpv6:
	gcc -o 04/udpv6 04/udpv6.c


ping: 
	gcc -o 05/ping 05/ping.c
