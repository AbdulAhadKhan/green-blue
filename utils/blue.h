#ifndef BLUE_H
#define BLUE_H

#include <errno.h>

#include "socket-object.h"

typedef int client_fd_t;

client_fd_t client_connection;

socket_fd_t initialize_server(port_number_t port_number);
int start_server(socket_fd_t server_socket, int (*callback)(void *), void *args);

#endif