#include <arpa/inet.h>

#include "utils/utils.h"
#include "utils/server-blue.h"
#include "utils/ANSI-colors.h"

port_number_t port_number = 2000;

static struct option long_options[] = {
    {"port",    required_argument,  0,  'p'},
    {0,         0,                  0,  0}
};

int parse_arguments(int argc, char *argv[]) {
    int opt;

    while ((opt = getopt_long(argc, argv, "p:cs", long_options, NULL)) != -1) {
        switch (opt) {
            case 'p': port_number = atoi(optarg); break;
            default: fprintf(stderr, "Usage: %s [-p port]\n", argv[0]); return -1;
        }
    }
    
    return 0;
}

int server_callback(void *args) {
    char buffer[1024];
    struct sockaddr_in client_address;
    socklen_t client_address_length = sizeof(client_address);
    
    getpeername(client_connection, (struct sockaddr *) &client_address, &client_address_length);
    char *client_ip = inet_ntoa(client_address.sin_addr);
    int client_port = ntohs(client_address.sin_port);
    
    while (read(client_connection, buffer, 1024) > 0) {
        printf("%s[%s] %s:%d:%s %s\n", ANSI_COLOR_BLUE, get_current_time_as_string(), 
               client_ip, client_port, ANSI_COLOR_RESET, buffer);
    }

    return 0;
}

int server_blue(port_number_t port_number) {
    socket_fd_t server_socket = initialize_server(port_number);
    start_server(server_socket, server_callback, NULL);
    return 0;
}

int main(int argc, char *argv[]) {
    parse_arguments(argc, argv);
    server_blue(port_number);
}