all: tcp tls tls_check tls_both

clean: 
	@rm -rf \
	01/tcp_client \
	01/tcp_server \
	02/tls_client \
	02/tls_server \
	04/tls_client_check \
	04/tls_server_check \
	05/tls_client_both \
	05/tls_server_both

tcp:
	gcc 01/tcp_client.c -o 01/tcp_client
	gcc 01/tcp_server.c -o 01/tcp_server

tls:
	gcc 02/tls_client.c -o 02/tls_client -lssl -lcrypto
	gcc 02/tls_server.c -o 02/tls_server -lssl -lcrypto

tls_check:
	gcc 04/tls_client_check.c -o 04/tls_client_check -lssl -lcrypto
	gcc 04/tls_server_check.c -o 04/tls_server_check -lssl -lcrypto

tls_both:
	gcc 05/tls_client_both.c -o 05/tls_client_both -lssl -lcrypto
	gcc 05/tls_server_both.c -o 05/tls_server_both -lssl -lcrypto
