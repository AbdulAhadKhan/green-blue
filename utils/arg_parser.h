#ifndef ARG_PARSER_H
#define ARG_PARSER_H

#include "meta.h"

typedef struct {
    peer_mode_t mode;
    port_number_t port;
} arguments_t;

arguments_t parse_arguments(int argc, char *argv[]);

#endif