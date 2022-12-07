#include <getopt.h>

#include "utils/meta.h"
#include "utils/socket-object.h"
#include "utils/blue.h"

config_t configs = {
    .port_number = 2000,
    .mode = SERVER
};

static struct option long_options[] = {
    {"port",    required_argument,  0,  'p'},
    {"client",  no_argument,        0,  'c'},
    {"server",  no_argument,        0,  's'},
    {0,         0,                  0,  0}
};

int parse_arguments(int argc, char *argv[]) {
    int opt;

    while ((opt = getopt_long(argc, argv, "p:cs", long_options, NULL)) != -1) {
        switch (opt) {
            case 'p': configs.port_number = atoi(optarg); break;
            case 'c': configs.mode = CLIENT; break;
            case 's': configs.mode = SERVER; break;
            default: fprintf(stderr, "Usage: %s [-p port] [-c | -s]\n", argv[0]); return -1;
        }
    }
    
    return 0;
}

int main(int argc, char *argv[]) {
    parse_arguments(argc, argv);
    socket_fd_t socket_fd = initialize_server(configs.port_number);

    printf("Starting server on port %d\n", configs.port_number);
    start_server(socket_fd, NULL, NULL);
}