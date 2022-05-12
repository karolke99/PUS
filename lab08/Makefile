all: cipher_ecb cipher_cbc cipher_bio digest mac server client rsa server_rsa client_rsa

clean:
	@rm -rf \
	01/cipher_ecb \
	02/cipher_cbc \
	03/cipher_bio \
	04/digest \
	05/mac \
	06/server \
	06/client \
	07/rsa \
	07/server_rsa \
	07/client_rsa

cipher_ecb:
	gcc -o 01/cipher_ecb -lcrypto 01/cipher_ecb.c

cipher_cbc:
	gcc -o 02/cipher_cbc -lcrypto 02/cipher_cbc.c

cipher_bio:
	gcc -o 03/cipher_bio -lcrypto 03/cipher_bio.c

digest:
	gcc -o 04/digest -lcrypto 04/digest.c

mac:
	gcc -o 05/mac -lssl 05/mac.c

server:
	gcc -o 06/server -lcrypto -lssl 06/server.c

client:
	gcc -o 06/client -lcrypto -lssl 06/client.c

rsa:
	gcc -o 07/rsa -lcrypto 07/rsa.c

server_rsa:
	gcc -o 07/server_rsa -lcrypto 07/server_rsa.c

client_rsa:
	gcc -o 07/client_rsa -lcrypto 07/client_rsa.c
