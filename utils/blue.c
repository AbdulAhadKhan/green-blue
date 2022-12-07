#include <errno.h>
#include <arpa/inet.h>
#include <time.h>
#include <signal.h>

#include "blue.h"
#include "meta.h"
#include "socket-object.h"
#include "ANSI-colors.h"

socket_fd_t server_socket_p;

/**
 * @brief Get the current time as string
 * 
 * @return char* 
 */
char * get_current_time_as_string() {
    time_t current_time;
    char *date_time_string = malloc(100);

    time(&current_time);
    strftime(date_time_string, 100, "%Y-%m-%d %H:%M:%S", localtime(&current_time));

    return date_time_string;
}

/**
 * @brief Log the connection to stdout
 * 
 * @param connection: connection file descriptor (int *)
 * 
 * @return int 
 */
int log_connection(int *connection) {
    struct sockaddr_in client_address;
    socklen_t client_address_length;
    
    getpeername(*connection, (struct sockaddr *) &client_address, &client_address_length);
    printf("%s[%s]%s Connection established with %s:%d\n",
           ANSI_COLOR_GREEN, get_current_time_as_string(), ANSI_COLOR_RESET, 
           inet_ntoa(client_address.sin_addr), ntohs(client_address.sin_port));
    
    return 0;
}

int log_server_start(socket_fd_t server_socket) {
    struct sockaddr_in server_address;
    socklen_t server_address_length;

    getsockname(server_socket, (struct sockaddr *) &server_address, &server_address_length);
    printf("%s[%s]%s Server started on port %d\n",
           ANSI_COLOR_GREEN, get_current_time_as_string(), ANSI_COLOR_RESET, 
           ntohs(server_address.sin_port));

    return 0;
}

/**
 * @brief Handle the connection fork
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

void close_connection(int signum) {
    close(server_socket_p);
    printf("%s[%s]%s Server closed\n", ANSI_COLOR_RED, get_current_time_as_string(), ANSI_COLOR_RESET);
    exit(0);
}

/**
 * @brief Initialize the server
 * 
 * @param configs: configurations (config_t *)
 * 
 * @return socket_fd_t 
 */
socket_fd_t initialize_server(config_t *configs) {
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in server_address = socket_object_init(configs->port_number);

    return bind(server_socket, (struct sockaddr *) &server_address, sizeof(server_address)) == 0 ? \
        server_socket : errno;
}

/**
 * @brief Start the server and listen for connections
 * 
 * @param server_socket: socket file descriptor (socket_fd_t)
 * @param callback: callback function (int (*)(void *))
 * @param args: arguments to be passed to the callback function (void *)
 * 
 * @return int 
 */
int start_server(socket_fd_t server_socket, int (*callback)(void *), void *args) {
    int connection;

    server_socket_p = server_socket;
    signal(SIGINT, close_connection);
    
    if (listen(server_socket, 5) == 0) {
        log_server_start(server_socket);
        while ((connection = accept(server_socket, NULL, NULL)) > 0)
            fork() == 0 ? connection_fork_handler(server_socket, &connection, callback, args) : close(connection);
    }

    return errno;
}