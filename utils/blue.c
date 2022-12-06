#include <errno.h>

#include "blue.h"
#include "meta.h"
#include "socket-object.h"

socket_fd_t initialize_server(config_t *configs) {
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in server_address = socket_object_init(configs->port_number);

    return bind(server_socket, (struct sockaddr *) &server_address, sizeof(server_address)) == 0 ? \
        server_socket : errno;
}

int start_server(int server_socket) {
    int connection;

    if (listen(server_socket, 5) == 0) {
        while ((connection = accept(server_socket, NULL, NULL)) > 0) {
            fprintf(stdout, "Connection established\n");
        }
    }

    return errno;
}