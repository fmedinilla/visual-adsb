#ifndef TCP_CLIENT_H_
#define TCP_CLIENT_H_


#ifdef WIN32

#include <winsock2.h>

typedef struct {
    char ip[17];
    int port;

    WSADATA wsaData;
    SOCKET clientSocket;
    struct sockaddr_in serverAddr;
} TCP_Client;

#else

#include <arpa/inet.h>

typedef int SOCKET;

typedef struct {
    char ip[17];
    int port;

    SOCKET clientSocket;
    struct sockaddr_in serverAddr;
} TCP_Client;

#endif


void client_init(TCP_Client *client, char *ip, int port);
void client_connect(TCP_Client *client);
void client_send(TCP_Client *client, char *message, int size);
void client_close(TCP_Client *client);

#endif