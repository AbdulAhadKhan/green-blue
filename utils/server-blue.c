#include <arpa/inet.h>
#include <string.h>

#include "utils.h"
#include "server-blue.h"
#include "ANSI-colors.h"

port_number_t port_number = 2000;

int server_blue_callback(void *args) {
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
    return server_socket < 0 || start_server(server_socket, server_blue_callback, NULL) < 0 ? -1 : 0;
}