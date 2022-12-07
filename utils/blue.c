#include <errno.h>
#include <arpa/inet.h>
#include <time.h>

#include "blue.h"
#include "meta.h"
#include "socket-object.h"

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"

char * get_current_time_as_string() {
    time_t current_time;
    char *date_time_string = malloc(100);

    time(&current_time);
    strftime(date_time_string, 100, "%Y-%m-%d %H:%M:%S", localtime(&current_time));

    return date_time_string;
}

int connection_fork_handler(socket_fd_t server_socket, int *connection) {
    close(server_socket);
    
    struct sockaddr_in client_address;
    socklen_t client_address_length;
    
    getpeername(*connection, (struct sockaddr *) &client_address, &client_address_length);
    printf("%s[%s]%s Connection established with %s:%d\n",
           ANSI_COLOR_GREEN, get_current_time_as_string(), ANSI_COLOR_RESET, 
           inet_ntoa(client_address.sin_addr), ntohs(client_address.sin_port));

    exit(0);
}

socket_fd_t initialize_server(config_t *configs) {
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in server_address = socket_object_init(configs->port_number);

    return bind(server_socket, (struct sockaddr *) &server_address, sizeof(server_address)) == 0 ? \
        server_socket : errno;
}

int start_server(socket_fd_t server_socket) {
    int connection;

    if (listen(server_socket, 5) == 0) {
        printf("Waiting for connection...\n");
        while ((connection = accept(server_socket, NULL, NULL)) > 0)
            fork() == 0 ? connection_fork_handler(server_socket, &connection) : close(connection);
    }

    return errno;
}