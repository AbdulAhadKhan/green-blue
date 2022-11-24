#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>

int server_socket;

void close_on_sigint(int sig) {
    printf("Closing connection and exiting...\n");
    if ((close(server_socket)) > -1)
        exit(0);
    else {
        printf("Error closing socket\n");
        exit(1);
    }
}

void main() {
    signal(SIGINT, close_on_sigint);
    char server_message[256] = "You have reached the server!";

    // Create the server socket
    server_socket = socket(AF_INET, SOCK_STREAM, 0);

    // Define the server address
    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(9002);
    server_address.sin_addr.s_addr = INADDR_ANY;

    // Catch and handle SIGINT

    // Bind the socket to our specified IP and port
    bind(server_socket, (struct sockaddr*) &server_address, sizeof(server_address));

    listen(server_socket, 5);

    int client_socket = accept(server_socket, NULL, NULL);

    // Send the message
    while (1)
        send(client_socket, server_message, sizeof(server_message), 0);
}