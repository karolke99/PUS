all: list ipaddr ipaddrmod event

clean:
	@rm -rf \
	01/list \
	02/ipaddr \
	03/ipaddrmod \
	04/event
	
list:
	gcc -o 01/list 01/list.c

ipaddr:
	gcc 02/ipaddr.c 02/libnetlink.c -o 02/ipaddr

ipaddrmod:
	gcc 03/ipaddrmod.c 03/libnetlink.c -o 03/ipaddrmod

event:
	gcc -o 04/event 04/event.c
