#ifndef META_H
#define META_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>

#define MESSAGE_SIZE 2048

typedef int port_number_t;
typedef int socket_fd_t;

typedef enum {
    SERVER,
    CLIENT
} peer_mode_t;

typedef enum {
    CONNECTED,
    DISCONNECTED
} status_t;

typedef struct {
    peer_mode_t mode;
    port_number_t port_number;
} config_t;

#endif