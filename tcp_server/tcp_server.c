#include <stdio.h>
#include <stdlib.h>
#include "tcp_server.h"

void server_init(TCP_Server *server, int port)
{
    server->port = port;

    // Inicializar Winsock
    if (WSAStartup(MAKEWORD(2, 2), &server->wsaData) != 0) {
        printf("Falló la inicialización de Winsock. Error: %d\n", WSAGetLastError());
        exit(EXIT_FAILURE);
    }

    // Crear socket
    server->serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (server->serverSocket == INVALID_SOCKET) {
        printf("No se pudo crear el socket. Error: %d\n", WSAGetLastError());
        WSACleanup();
        exit(EXIT_FAILURE);
    }

    // Configurar address
    server->address.sin_family = AF_INET;
    server->address.sin_addr.S_un.S_addr = INADDR_ANY;
    server->address.sin_port = htons(server->port);

    // Vincular el socket
    int r_bin = bind(server->serverSocket, (const struct sockaddr*)&server->address, sizeof(server->address));
    if (r_bin == SOCKET_ERROR) {
        printf("Falló la vinculación del socket. Error: %d\n", WSAGetLastError());
        closesocket(server->serverSocket);
        WSACleanup();
        exit(EXIT_FAILURE);
    }
}

void server_listen(TCP_Server *server)
{
    // Escuchar conexiones entrantes
    int r_listen = listen(server->serverSocket, 3);
    if (r_listen == SOCKET_ERROR) {
        printf("Falló la escucha del socket. Error: %d\n", WSAGetLastError());
        closesocket(server->serverSocket);
        WSACleanup();
        exit(EXIT_FAILURE);
    }

    printf("Esperando conexiones entrantes...\n");

    // Aceptar una conexión entrante
    int addrLen = sizeof(struct sockaddr_in);
    server->clientSocket = accept(server->serverSocket, &server->client_addr, &addrLen);
    if (server->clientSocket == INVALID_SOCKET) {
        printf("Fallo al aceptar la conexión. Error: %d\n", WSAGetLastError());
        closesocket(server->serverSocket);
        WSACleanup();
        exit(EXIT_FAILURE);
    }

    printf("Conexion aceptada.\n");
}

void server_receive(TCP_Server *server, char *buffer, int size)
{
    // Recibir datos del cliente
    int recvSize = recv(server->clientSocket, buffer, size, 0);
    buffer[recvSize] = '\0';
}

void server_close_client(TCP_Server *server)
{
    // Cerrar el socket del cliente
    closesocket(server->clientSocket);
}

void server_close(TCP_Server *server)
{
    // Cerrar el socket del servidor
    closesocket(server->serverSocket);

    // Finalizar el uso de Winsock
    WSACleanup();
}
