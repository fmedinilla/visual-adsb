SERVER_APP=tcp_server
CLIENT_APP=tcp_client


all: windows


windows: win_server


win_server:
	@gcc -o $(SERVER_APP) win_server.c -lm -lws2_32
