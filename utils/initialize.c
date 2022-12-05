#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>

#include "initialize.h"
    
static struct option long_options[] = {
    {"port",    required_argument,  0,  'p'},
    {"client",  no_argument,        0,  'c'},
    {"server",  no_argument,        0,  's'},
    {0,         0,                  0,  0}
};

int parse_arguments(int argc, char *argv[], config_t *configs) {
    int opt;

    while ((opt = getopt_long(argc, argv, "p:cs", long_options, NULL)) != -1) {
        switch (opt) {
            case 'p': configs->port_number = atoi(optarg); break;
            case 'c': configs->mode = CLIENT; break;
            case 's': configs->mode = SERVER; break;
            default: fprintf(stderr, "Usage: %s [-p port] [-c | -s]\n", argv[0]); return -1;
        }
    }
    
    return 0;
}