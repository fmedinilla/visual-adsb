SERVER_APP=server
CLIENT_APP=client


all: unix

windows: win_server win_client
unix: unix_server unix_client


win_server:
	@gcc -o $(SERVER_APP) server.c win_tcp_server.c -lm -lws2_32


win_client:
	@gcc -o $(CLIENT_APP) client.c win_tcp_client.c -lm -lws2_32


unix_server:
	@gcc -o $(SERVER_APP) server.c unix_tcp_server.c -lm


unix_client:
	@gcc -o $(CLIENT_APP) client.c unix_tcp_client.c -lm