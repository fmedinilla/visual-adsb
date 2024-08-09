#include <stdio.h>
#include <winsock2.h>


#define PORT 8080


int main() {
    WSADATA wsaData;
    SOCKET clientSocket;
    struct sockaddr_in serverAddr;
    char *message;

    // Inicializar Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        printf("Falló la inicialización de Winsock. Error: %d\n", WSAGetLastError());
        return 1;
    }

    // Crear un socket
    clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == INVALID_SOCKET) {
        printf("No se pudo crear el socket. Error: %d\n", WSAGetLastError());
        WSACleanup();
        return 1;
    }

    // Configurar la estructura sockaddr_in para conectar al servidor
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = inet_addr("192.168.1.120");    // Dirección IP del servidor
    serverAddr.sin_port = htons(PORT);                          // Puerto del servidor

    // Conectar al servidor
    if (connect(clientSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0) {
        printf("Error al conectar con el servidor. Error: %d\n", WSAGetLastError());
        closesocket(clientSocket);
        WSACleanup();
        return 1;
    }

    printf("Conectado al servidor.\n");

    // Enviar un mensaje al servidor
    message = "Hola desde el cliente";
    if (send(clientSocket, message, strlen(message), 0) < 0) {
        printf("Error al enviar datos. Error: %d\n", WSAGetLastError());
        closesocket(clientSocket);
        WSACleanup();
        return 1;
    }

    printf("Mensaje enviado: %s\n", message);

    // Cerrar el socket
    closesocket(clientSocket);

    // Finalizar el uso de Winsock
    WSACleanup();

    return 0;
}
