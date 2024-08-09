#include <stdio.h>
#include <winsock2.h>


#define PORT 8080


int main(int argc, char *argv[])
{
    WSADATA wsaData;

    // Inicializar Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        printf("Falló la inicialización de Winsock. Error: %d\n", WSAGetLastError());
        exit(EXIT_FAILURE);
    }

    // Crear socket
    SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == INVALID_SOCKET) {
        printf("No se pudo crear el socket. Error: %d\n", WSAGetLastError());
        WSACleanup();
        exit(EXIT_FAILURE);
    }

    // Configurar address
    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.S_un.S_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Vincular el socket
    int r_bin = bind(serverSocket, (const struct sockaddr*)&address, sizeof(address));
    if (r_bin == SOCKET_ERROR) {
        printf("Falló la vinculación del socket. Error: %d\n", WSAGetLastError());
        closesocket(serverSocket);
        WSACleanup();
        exit(EXIT_FAILURE);
    }

    int requests = 0;
    while (1) {
        if (requests >= 3) break;

        // Escuchar conexiones entrantes
        int r_listen = listen(serverSocket, 3);
        if (r_listen == SOCKET_ERROR) {
            printf("Falló la escucha del socket. Error: %d\n", WSAGetLastError());
            closesocket(serverSocket);
            WSACleanup();
            exit(EXIT_FAILURE);
        }

        printf("Esperando conexiones entrantes...\n");

        int addrLen = sizeof(struct sockaddr_in);

        // Aceptar una conexión entrante
        struct sockaddr clientAddr;

        SOCKET clientSocket = accept(serverSocket, &clientAddr, &addrLen);
        if (clientSocket == INVALID_SOCKET) {
            printf("Fallo al aceptar la conexión. Error: %d\n", WSAGetLastError());
            closesocket(serverSocket);
            WSACleanup();
            exit(EXIT_FAILURE);
        }

        printf("Conexion aceptada.\n");

        // Recibir datos del cliente
        char buffer[1024];
        int recvSize = recv(clientSocket, buffer, 1024, 0);
        buffer[recvSize] = '\0';
        printf("Mensaje recibido: %s\n", buffer);

        // Cerrar el socket del cliente
        closesocket(clientSocket);

        requests++;
    }

    // Cerrar el socket del servidor
    closesocket(serverSocket);

    // Finalizar el uso de Winsock
    WSACleanup();

    return 0;
}