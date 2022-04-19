all: iolist proclist ioarp ioaddr

clean:
	@rm -rf \
	01/iolist \
	02/proclist \
	03/ioarp \
	05/ioaddr
	
iolist:
	gcc -o 01/iolist 01/iolist.c
	
proclist:
	gcc -o 02/proclist 02/proclist.c
	
ioarp:
	gcc -o 03/ioarp 03/ioarp.c

ioaddr:
	gcc -o 05/ioaddr 05/ioaddr.c
	
