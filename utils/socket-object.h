#ifndef SOCKET_OBJECT_H
#define SOCKET_OBJECT_H

#include <sys/socket.h>

#include "meta.h"

const struct socaddr * socket_object_init(port_number_t port);

#endif