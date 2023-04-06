#ifndef SOCKET_OBJECT_H
#define SOCKET_OBJECT_H

#include "meta.h"

struct sockaddr_in socket_object_init(char *host, port_number_t port);

#endif