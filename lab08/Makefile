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
	gcc -o 01/cipher_ecb 01/cipher_ecb.c -lcrypto

cipher_cbc:
	gcc -o 02/cipher_cbc 02/cipher_cbc.c -lcrypto

cipher_bio:
	gcc -o 03/cipher_bio 03/cipher_bio.c -lcrypto

digest:
	gcc -o 04/digest 04/digest.c -lcrypto

mac:
	gcc -o 05/mac 05/mac.c -lcrypto -lssl

server:
	gcc -o 06/server 06/server.c -lcrypto -lssl

client:
	gcc -o 06/client 06/client.c -lcrypto -lssl

rsa:
	gcc -o 07/rsa 07/rsa.c -lcrypto

server_rsa:
	gcc -o 07/server_rsa 07/server_rsa.c -lcrypto

client_rsa:
	gcc -o 07/client_rsa 07/client_rsa.c -lcrypto
