#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <string.h>
#include <signal.h>

#include <pthread.h>
#include <sys/socket.h>

#include "utils/meta.h"
#include "utils/ANSI-colors.h"
#include "utils/socket-object.h"

socket_fd_t server_socket;

struct config  {
    int server_port;
    char *address;
};

struct config configs = {
    .server_port = 2000,
    .address = "127.0.0.1"
};

static struct option long_options[] = {
    {"server-port",  no_argument,        0,  'p'},
    {"address",   no_argument,           0,  'a'},
    {0,              0,                  0,  0}
};

/**
 * @brief Invoked when the program is given a SIGINT signal.
 * Closes the server socket and exits the program.
 * 
 * @param sig 
 */
void interrupt_handler(int sig) {
    printf("%s[-]%s Disconnecting from server: %s:%d\n", 
            ANSI_COLOR_RED, ANSI_COLOR_RESET, configs.address, configs.server_port);
    close(server_socket);
    exit(0);
}

/**
 * @brief Thread function that checks if the connection is still alive
 * every second. If the connection is closed, the program is given a
 * SIGINT signal.
 * 
 * @param args 
 * @return void* 
 */
void * check_connection_thread(void *args) {
    socket_fd_t connection = *(socket_fd_t *) args;
    
    while (recv(connection, NULL, 0, MSG_PEEK | MSG_DONTWAIT) != 0) sleep(1);
    
    printf("%s\n[-]%s Connection closed by server\n", ANSI_COLOR_RED, ANSI_COLOR_RESET);
    kill(getpid(), SIGINT);

    return NULL;
}

/**
 * @brief Parse the arguments passed to the program and set the
 * configuration variables.
 * 
 * @param argc 
 * @param argv 
 * @param config 
 * @return int 
 */
int parse_arguments(int argc, char *argv[], struct config *config) {
    int opt;

    while ((opt = getopt_long(argc, argv, "p:a:", long_options, NULL)) != -1) {
        switch (opt) {
            case 'p': config->server_port = atoi(optarg); break;
            case 'a': config->address = optarg; break;
            default: fprintf(stderr, "Usage: %s [-p port] [-s port]\n", argv[0]); return -1;
        }
    }
    
    return 0;
}

/**
 * @brief Get the message from the user and send it to the server.
 * Then, get the response from the server and print it. If the user
 * types "quit", the program will be terminated by sending a SIGINT signal.
 * 
 * @param socket: socket file descriptor (socket_fd_t)
 */
void process_communication(socket_fd_t socket) {
    int read_size;
    char buffer[MESSAGE_SIZE];
    
    while (1) {
        printf("Message: ");
        fgets(buffer, sizeof(buffer), stdin);
        if (strcmp(buffer, "quit\n") == 0) 
            kill(getpid(), SIGINT);
        send(socket, buffer, sizeof(buffer), 0);
        read_size = recv(socket, &buffer, sizeof(buffer), 0);
        buffer[read_size] = '\0';
        printf("Server response:\n%s", buffer);
    }
}

/**
 * @brief Create a client socket and connect to the server.
 * If the connection is successful, start a thread to check the connection status
 * and start the communication process. Otherwise, print an error message and return.
 * 
 */
void green_client() {
    pthread_t thread;
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in server_address = socket_object_init(configs.address, configs.server_port);

    if (connect(server_socket, (struct sockaddr *) &server_address, sizeof(server_address)) != -1) {
        pthread_create(&thread, NULL, check_connection_thread, &server_socket);
        printf("%s[+]%s Connected to server: %s:%d\n", ANSI_COLOR_GREEN, 
               ANSI_COLOR_RESET, configs.address, configs.server_port);
        process_communication(server_socket);        
    }

    fprintf(stderr, "%s[-]%s Failed to connect to server: %s:%d\n", 
            ANSI_COLOR_RED, ANSI_COLOR_RESET, configs.address, configs.server_port);
}

/**
 * @brief Start the client application
 * 
 * @param argc 
 * @param argv 
 * @return int 
 */
int main(int argc, char *argv[]) {
    signal(SIGINT, interrupt_handler);
    parse_arguments(argc, argv, &configs);
    green_client();
}