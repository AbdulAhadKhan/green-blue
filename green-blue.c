#include "utils/meta.h"
#include "utils/initialize.h"
#include "utils/socket-object.h"

config_t configs = {
    .port_number = 2000,
    .mode = SERVER
};

int main(int argc, char *argv[]) {
    parse_arguments(argc, argv, &configs);

    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in server_address = socket_object_init(configs.port_number);
    bind(server_socket, (struct sockaddr *) &server_address, sizeof(server_address));
}