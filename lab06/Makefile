all: list ipaddr

clean:
	@rm -rf \
	01/list \
	02/ipaddr
	
list:
	gcc -o 01/list 01/list.c

ipaddr:
	gcc 02/ipaddr.c 02/libnetlink.c -o 02/ipaddr


