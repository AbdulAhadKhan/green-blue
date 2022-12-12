#ifndef SERVER_BLUE_H
#define SERVER_BLUE_H

#include <getopt.h>

#include "meta.h"
#include "blue.h"

int server_blue_callback(void *args);
int server_blue(socket_fd_t server_socket);

#endif