#ifndef TCP_SERVER_H_
#define TCP_SERVER_H_

#include <winsock2.h>

typedef struct {
    int port;
    WSADATA wsaData;
    SOCKET serverSocket;
    struct sockaddr_in address;
    SOCKET clientSocket;
    struct sockaddr client_addr;
} TCP_Server;


void server_init(TCP_Server *server, int port);
void server_listen(TCP_Server *server);
void server_receive(TCP_Server *server, char *buffer, int size);
void server_close_client(TCP_Server *server);
void server_close(TCP_Server *server);

#endif