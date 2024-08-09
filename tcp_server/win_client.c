#include <stdio.h>
#include <winsock2.h>


#define PORT 8080


typedef struct {
    char ip[17];
    int port;

    WSADATA wsaData;
    SOCKET clientSocket;
    struct sockaddr_in serverAddr;
} TCP_Client;


void client_init(TCP_Client *client, char *ip, int port);
void client_connect(TCP_Client *client);
void client_send(TCP_Client *client, char *message, int size);
void client_close(TCP_Client *client);

int main()
{
    TCP_Client client;    

    client_init(&client, "192.168.1.120", PORT);

    client_connect(&client);

    char *message = "Hola desde el cliente";
    client_send(&client, message, strlen(message));
    printf("Mensaje enviado: %s\n", message);

    client_close(&client);

    return 0;
}

void client_init(TCP_Client *client, char *ip, int port)
{
    client->port = port;
    strcpy(client->ip, ip);

    // Inicializar Winsock
    if (WSAStartup(MAKEWORD(2, 2), &client->wsaData) != 0) {
        printf("Falló la inicialización de Winsock. Error: %d\n", WSAGetLastError());
        exit(EXIT_FAILURE);
    }

    // Crear un socket
    client->clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (client->clientSocket == INVALID_SOCKET) {
        printf("No se pudo crear el socket. Error: %d\n", WSAGetLastError());
        WSACleanup();
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
        printf("Error al conectar con el servidor. Error: %d\n", WSAGetLastError());
        closesocket(client->clientSocket);
        WSACleanup();
        exit(EXIT_FAILURE);
    }

    printf("Conectado al servidor.\n");
}
void client_send(TCP_Client *client, char *message, int size)
{
    // Enviar un mensaje al servidor
    if (send(client->clientSocket, message, size, 0) < 0) {
        printf("Error al enviar datos. Error: %d\n", WSAGetLastError());
        closesocket(client->clientSocket);
        WSACleanup();
        exit(EXIT_FAILURE);
    }
}
void client_close(TCP_Client *client)
{
     // Cerrar el socket
    closesocket(client->clientSocket);

    // Finalizar el uso de Winsock
    WSACleanup();
}