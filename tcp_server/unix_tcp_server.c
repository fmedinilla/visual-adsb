#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>
#include "tcp_server.h"

const int INVALID_SOCKET = 0;
const int SOCKET_ERROR = 0;

void server_init(TCP_Server *server, int port)
{
    server->port = port;

    // crear socket
    server->serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (server->serverSocket == INVALID_SOCKET) {
        printf("No se pudo crear el socket. Error: %d\n", 1);
        exit(EXIT_FAILURE);
    }

    // configurar address
    server->address.sin_family = AF_INET;
    server->address.sin_addr.s_addr = INADDR_ANY;
    server->address.sin_port = htons(server->port);

    // vincular socket
    int r_bin = bind(server->serverSocket, (const struct sockaddr*)&server->address, sizeof(server->address));
    if (r_bin < SOCKET_ERROR) {
        printf("No se pudo vincular el socket. Error: %d\n", 2);
        exit(EXIT_FAILURE);
    }
}

void server_listen(TCP_Server *server)
{
   // escuchar conexiones entrantes
    int r_listen = listen(server->serverSocket, 3);
    if (r_listen < SOCKET_ERROR) {
        printf("Error en la  escucha del socket. Error %d\n", 3);
        close(server->serverSocket);
        exit(EXIT_FAILURE);
    }

   // aceptar conexion entrante
   int addrlen = sizeof(struct sockaddr);
   server->clientSocket = accept(server->serverSocket, &server->client_addr, &addrlen);
   if (server->clientSocket == INVALID_SOCKET) {
        printf("Error al aceptar la conexion. Error %d\n", 4);
        close(server->serverSocket);
        exit(EXIT_FAILURE);
   }

   printf("Conexion aceptad.\n");
}

void server_receive(TCP_Server *server, char *buffer, int size)
{
    // recibir datos del cliente
    int recvSize = recv(server->clientSocket, buffer, size, 0);
    buffer[recvSize] = '\0';
}

void server_close_client(TCP_Server *server)
{
    // cerrar el socket del cliente
    close(server->clientSocket);
}

void server_close(TCP_Server *server)
{
    // cerrar socket del servidor
    close(server->serverSocket);
}
