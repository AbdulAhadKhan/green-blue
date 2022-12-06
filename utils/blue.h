#ifndef BLUE_H
#define BLUE_H

#include <errno.h>

#include "socket-object.h"

socket_fd_t initialize_server(config_t *configs);
int start_server(int server_socket);

#endif