#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <getopt.h>

#include "meta.h"

port_number_t port_number = 2000;
peer_mode_t mode = SERVER;

void parse_args(int argc, char *argv[]) {
    int opt;
    static struct option long_options[] = {
        {"port",    required_argument,  0,  'p'},
        {"client",  no_argument,        0,  'c'},
        {"server",  no_argument,        0,  's'},
        {0,         0,                  0,  0}
    };

    while ((opt = getopt_long(argc, argv, "p:cs", long_options, NULL)) != -1) {
        switch (opt) {
            case 'p': port_number = atoi(optarg); break;
            case 'c': mode = CLIENT; break;
            case 's': mode = SERVER; break;
            default: fprintf(stderr, "Usage: %s [-p port] [-c | -s]\n", argv[0]); exit(1);
        }
    }   
}

int main(int argc, char *argv[]) {
    parse_args(argc, argv);
    printf("Port: %d\n", port_number);
    printf("Mode: %s\n", mode == CLIENT ? "CLIENT" : "SERVER");
    return 0;
}