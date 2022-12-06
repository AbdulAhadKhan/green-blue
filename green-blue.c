#include "utils/meta.h"
#include "utils/initialize.h"
#include "utils/socket-object.h"
#include "utils/blue.h"

config_t configs = {
    .port_number = 2000,
    .mode = SERVER
};

int main(int argc, char *argv[]) {
    parse_arguments(argc, argv, &configs);
    socket_fd_t socket_fd = initialize_server(&configs);
    start_server(socket_fd);
}