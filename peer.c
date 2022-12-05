#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <getopt.h>

#include "utils/meta.h"
#include "utils/arg-parser.h"

config_t configs;

int main(int argc, char *argv[]) {
    configs = parse_arguments(argc, argv);

    printf("Port number: %d\n", configs.port_number);
    printf("Mode: %s\n", configs.mode == SERVER ? "SERVER" : "CLIENT");
}