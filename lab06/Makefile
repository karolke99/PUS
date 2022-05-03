all: list ipaddr ipaddrmod

clean:
	@rm -rf \
	01/list \
	02/ipaddr \
	03/ipaddrmod
	
list:
	gcc -o 01/list 01/list.c

ipaddr:
	gcc 02/ipaddr.c 02/libnetlink.c -o 02/ipaddr

ipaddrmod:
	gcc 03/ipaddrmod.c 03/libnetlink.c -o 03/ipaddrmod


