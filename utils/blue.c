#include <errno.h>
#include <arpa/inet.h>
#include <signal.h>
#include <string.h>

#include "blue.h"
#include "meta.h"
#include "utils.h"
#include "socket-object.h"
#include "ANSI-colors.h"

socket_fd_t server_socket_p;

/**
 * @brief Log the connection to stdout
 * 
 * @param connection: connection file descriptor (int *)
 * 
 * @return int 
 */
int log_connection(socket_fd_t *connection) {
    struct sockaddr_in client_address;
    socklen_t client_address_length = sizeof(client_address);
    
    getpeername(*connection, (struct sockaddr *) &client_address, &client_address_length);
    printf("%s[%s]%s Connection established with %s:%d\n",
           ANSI_COLOR_GREEN, get_current_time_as_string(), ANSI_COLOR_RESET, 
           inet_ntoa(client_address.sin_addr), ntohs(client_address.sin_port));
    
    return 0;
}

/**
 * @brief Log the server start to stdout
 * 
 * @param server_socket 
 * @return int 
 */
int log_server_start(socket_fd_t server_socket) {
    struct sockaddr_in server_address;
    socklen_t server_address_length = sizeof(server_address);

    getsockname(server_socket, (struct sockaddr *) &server_address, &server_address_length);
    printf("%s[%s]%s Server started on port: %d\n",
           ANSI_COLOR_GREEN, get_current_time_as_string(), ANSI_COLOR_RESET,  ntohs(server_address.sin_port));

    return 0;
}

/**
 * @brief Gets called every time a new connection is established.
 * Since this is a forked process, it executes the callback function
 * inside the child process and exits.
 * 
 * @param server_socket: socket file descriptor (socket_fd_t)
 * @param connection: connection file descriptor (int *)
 * @param callback: callback function (int (*)(void *))
 * @param args: arguments to be passed to the callback function (void *)
 * 
 * @return int 
 */
int connection_fork_handler(socket_fd_t server_socket, int *connection, int (*callback)(void *), void *args) {
    close(server_socket);
    log_connection(connection);
    if (callback != NULL)
        callback(args);
    exit(0);
}

/**
 * @brief Close the server socket and exit.
 * 
 * @param signum 
 */
void close_connection(int signum) {
    close(server_socket_p);
    printf("%s[%s]%s %s[PORT %d]%s Server closed\n", 
           ANSI_COLOR_RED, get_current_time_as_string(), ANSI_COLOR_RESET,
           ANSI_COLOR_YELLOW, server_socket_p, ANSI_COLOR_RESET);
    exit(0);
}

/**
 * @brief Initialize the server socket and bind it to the port number
 * 
 * @param port_number 
 * @return socket_fd_t
 */
socket_fd_t initialize_server(port_number_t port_number) {
    server_socket_p = port_number;
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in server_address = socket_object_init(port_number);

    return bind(server_socket, (struct sockaddr *) &server_address, sizeof(server_address)) == 0 ? \
        server_socket : -1;
}

/**
 * @brief Start the server and listen for connections.
 * Every connection is handled in a fork.
 * 
 * @param server_socket: socket file descriptor (socket_fd_t)
 * @param callback: callback function (int (*)(void *))
 * @param args: arguments to be passed to the callback function (void *)
 * 
 * @return int 
 */
int start_server(socket_fd_t server_socket, int (*callback)(void *), void *args) {
    signal(SIGINT, close_connection);
    
    if (listen(server_socket, 5) < 0)
        return -1;
    
    log_server_start(server_socket);
    while ((client_connection = accept(server_socket, NULL, NULL)) > 0)
        fork() == 0 ? connection_fork_handler(server_socket, &client_connection, callback, args) : close(client_connection);
    
    return 0;
}