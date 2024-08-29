#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "tcp_client.h"

const int INVALID_SOCKET = 0;
const int SOCKET_ERROR = 0;

void client_init(TCP_Client *client, char *ip, int port)
{
    client->port = port;
    strcpy(client->ip, ip);

    // Crear un socket
    client->clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (client->clientSocket == INVALID_SOCKET) {
        printf("No se pudo crear el socket. Error: %d\n", 1);
        exit(EXIT_FAILURE);
    }

    // Configurar la estructura sockaddr_in para conectar al servidor
    client->serverAddr.sin_family = AF_INET;
    client->serverAddr.sin_addr.s_addr = inet_addr(client->ip); // Dirección IP del servidor
    client->serverAddr.sin_port = htons(client->port);          // Puerto del servidor
}

void client_connect(TCP_Client *client)
{
    // Conectar al servidor
    if (connect(client->clientSocket, (struct sockaddr *)&client->serverAddr, sizeof(client->serverAddr)) < 0) {
        printf("Error al conectar con el servidor. Error: %d\n", 2);
        close(client->clientSocket);
        exit(EXIT_FAILURE);
    }

    printf("Conectado al servidor.\n");
}

void client_send(TCP_Client *client, char *message, int size)
{
    // Enviar un mensaje al servidor
    if (send(client->clientSocket, message, size, 0) < 0) {
        printf("Error al enviar datos. Error: %d\n", 3);
        close(client->clientSocket);
        exit(EXIT_FAILURE);
    }
}

void client_close(TCP_Client *client)
{
     // Cerrar el socket
    close(client->clientSocket);
}
