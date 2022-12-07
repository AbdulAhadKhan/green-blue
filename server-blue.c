#include <getopt.h>

#include "utils/meta.h"
#include "utils/blue.h"

port_number_t port_number = 2000;

static struct option long_options[] = {
    {"port",    required_argument,  0,  'p'},
    {0,         0,                  0,  0}
};

int parse_arguments(int argc, char *argv[], config_t *configs) {
    int opt;

    while ((opt = getopt_long(argc, argv, "p:cs", long_options, NULL)) != -1) {
        switch (opt) {
            case 'p': port_number = atoi(optarg); break;
            default: fprintf(stderr, "Usage: %s [-p port]\n", argv[0]); return -1;
        }
    }
    
    return 0;
}

int main(int argc, char *argv[]) {
    printf("Hello, world!\n");
}