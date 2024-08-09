SERVER_APP=server
CLIENT_APP=client


all: windows


windows: win_server win_client


win_server:
	@gcc -o $(SERVER_APP) win_server.c tcp_server.c -lm -lws2_32


win_client:
	@gcc -o $(CLIENT_APP) win_client.c -lm -lws2_32