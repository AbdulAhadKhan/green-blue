#include "socket-object.h"

struct sockaddr_in socket_object_init(port_number_t port) {
    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(port);
    server_address.sin_addr.s_addr = INADDR_ANY;
    return server_address;
}