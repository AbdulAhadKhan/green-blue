#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>

int network_socket;

void close_on_sigint(int sig) {
    printf("Closing connection and exiting...\n");
    if ((close(network_socket)) > -1)
        exit(0);
    else {
        printf("Error closing socket\n");
        exit(1);
    }
}

void main() {
    signal(SIGINT, close_on_sigint);
    
    //create a socket
    network_socket = socket(AF_INET, SOCK_STREAM, 0);

    //specify an address for the socket
    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(9002);
    server_address.sin_addr.s_addr = INADDR_ANY;

    int connection_status = connect(network_socket, (struct sockaddr *) &server_address, sizeof(server_address));
    //check for error with the connection
    if (connection_status == -1) {
        printf("There was an error making a connection to the remote socket \n\n");
    }

    //receive data from the server
    char server_response[256];
    char client_message[256];
    while (1) {
        recv(network_socket, &server_response, sizeof(server_response), 0);
        printf("Server: %s\n", server_response);
        
        printf("Message: ");
        fgets(client_message, sizeof(client_message), stdin);
        client_message[strlen(client_message) - 1] = '\0';
        send(network_socket, client_message, sizeof(client_message), 0);
    }
}