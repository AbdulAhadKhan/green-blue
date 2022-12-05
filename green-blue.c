#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <getopt.h>

#include "utils/meta.h"
#include "utils/initialize.h"

config_t configs = {
    .port_number = 2000,
    .mode = SERVER
};

int main(int argc, char *argv[]) {
    parse_arguments(argc, argv, &configs);

    printf("Port number: %d\n", configs.port_number);
    printf("Mode: %s\n", configs.mode == SERVER ? "SERVER" : "CLIENT");
}