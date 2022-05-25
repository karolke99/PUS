all: suidserver rootserver daemon fserver

clean: 
	@rm -fr \
	01/suidserver \
	02/rootserver \
	03/daemon \
	04/fserver


suidserver:
	gcc -o 01/suidserver 01/suidserver.c

rootserver:
	gcc -o 02/rootserver 02/rootserver.c

daemon:
	gcc -o 03/daemon 03/daemon.c

fserver:
	gcc -o 04/fserver 04/fserver.c

