#include <getopt.h>

#include "utils/meta.h"
#include "utils/socket-object.h"
#include "utils/server-blue.h"

port_number_t port_number = 4000;

static struct option long_options[] = {
    {"port",    required_argument,  0,  'p'},
    {0,         0,                  0,  0}
};

int parse_arguments(int argc, char *argv[]) {
    int opt;

    while ((opt = getopt_long(argc, argv, "p:", long_options, NULL)) != -1) {
        switch (opt) {
            case 'p': port_number = atoi(optarg); break;
            default: fprintf(stderr, "Usage: %s [-p port] [-c | -s]\n", argv[0]); return -1;
        }
    }
    
    return 0;
}

int main(int argc, char *argv[]) {
    parse_arguments(argc, argv);
    socket_fd_t socket_fd = initialize_server(port_number);
    server_blue(port_number, socket_fd);
}