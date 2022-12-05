#ifndef META_H
#define META_H

typedef int port_number_t;

typedef enum {
    SERVER,
    CLIENT
} peer_mode_t;

typedef struct {
    peer_mode_t mode;
    port_number_t port_number;
} config_t;

#endif