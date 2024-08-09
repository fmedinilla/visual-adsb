#include <stdio.h>
#include "tcp_client.h"


int main()
{
    TCP_Client client;    

    client_init(&client, "192.168.1.120", 8080);

    client_connect(&client);

    char *message = "Hola desde el cliente";
    client_send(&client, message, strlen(message));
    printf("Mensaje enviado: %s\n", message);

    client_close(&client);

    return 0;
}

