#include <stdio.h>
#include "tcp_server.h"


int main(int argc, char *argv[])
{
    TCP_Server server;
    server_init(&server, 8080);

    int requests = 0;
    while (1) {
        if (requests >= 3) break;

        server_listen(&server);

        char buffer[1024];
        server_receive(&server, buffer, 1024);
        printf("Received: %s\n", buffer);

        server_close_client(&server);

        requests++;
    }

    server_close(&server);

    return 0;
}
