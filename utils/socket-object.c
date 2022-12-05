#include <stdio.h>
#include <stdlib.h>

#include "socket-object.h"

const struct socaddr * socket_object_init(port_number_t port)) {
    if (! (int sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        struct sockaddr_in server_address;
        server_address.sin_family = AF_INET;
        server_address.sin_port = htons(port);
        server_address.sin_addr.s_addr = INADDR_ANY;
        return (const struct socaddr *) &server_address;
    } return NULL;
}