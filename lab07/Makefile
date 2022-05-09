all: authlist exec exec_rsa sftp sftp_get
clean:
	@rm -rf \
	01/authlist \
	01/libcommon.o \
	02/exec \
	02/libcommon.o \
	03/exec_rsa \
	03/libcommon.o \
	04/sftp \
	04/libcommon.o \
	05/sftp_get \
	05/libcommon.o

	
authlist:
	gcc -c 01/libcommon.c -o 01/libcommon.o
	gcc 01/authlist.c -lssh2 01/libcommon.o -o 01/authlist

exec:
	gcc -c 02/libcommon.c -o 02/libcommon.o
	gcc 02/exec.c -lssh2 02/libcommon.o -o 02/exec

exec_rsa:
	gcc -c 03/libcommon.c -o 03/libcommon.o
	gcc 03/exec_rsa.c -lssh2 03/libcommon.o -o 03/exec_rsa

sftp:
	gcc -c 04/libcommon.c -o 04/libcommon.o
	gcc 04/sftp.c -lssh2 04/libcommon.o -o 04/sftp

sftp_get:
	gcc -c 05/libcommon.c -o 05/libcommon.o
	gcc 05/sftp_get.c -lssh2 05/libcommon.o -o 05/sftp_get