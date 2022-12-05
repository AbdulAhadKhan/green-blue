#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>

#include "arg-parser.h"

config_t parse_arguments(int argc, char *argv[]) {
    int opt;
    config_t args = {
        .port_number = 2000,
        .mode = SERVER
    };
    
    static struct option long_options[] = {
        {"port",    required_argument,  0,  'p'},
        {"client",  no_argument,        0,  'c'},
        {"server",  no_argument,        0,  's'},
        {0,         0,                  0,  0}
    };

    while ((opt = getopt_long(argc, argv, "p:cs", long_options, NULL)) != -1) {
        switch (opt) {
            case 'p': args.port_number = atoi(optarg); break;
            case 'c': args.mode = CLIENT; break;
            case 's': args.mode = SERVER; break;
            default: fprintf(stderr, "Usage: %s [-p port] [-c | -s]\n", argv[0]); exit(EXIT_FAILURE);
        }
    }
    
    return args;
}