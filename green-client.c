#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <string.h>
#include <signal.h>

#include <pthread.h>
#include <sys/mman.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

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


void * check_connection_thread(void *args) {
    socket_fd_t connection = *(socket_fd_t *) args;
    
    while (recv(connection, NULL, 0, MSG_PEEK | MSG_DONTWAIT) != 0) sleep(1);
    
    printf("%s\n[-]%s Connection closed by server\n", ANSI_COLOR_RED, ANSI_COLOR_RESET);
    exit(0);
}

void quit(int sig) {
    printf("%s[-]%s Disconnecting from server: %s:%d\n", 
            ANSI_COLOR_RED, ANSI_COLOR_RESET, configs.address, configs.server_port);
    close(server_socket);
    exit(0);
}

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

void green_client() {
    pthread_t thread;
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in server_address = socket_object_init(configs.address, configs.server_port);

    if (connect(server_socket, (struct sockaddr *) &server_address, sizeof(server_address)) != -1) {
        pthread_create(&thread, NULL, check_connection_thread, &server_socket);
        printf("%s[+]%s Connected to server: %s:%d\n", ANSI_COLOR_GREEN, ANSI_COLOR_RESET, configs.address, configs.server_port);
        process_communication(server_socket);        
    }

    fprintf(stderr, "%s[-]%s Failed to connect to server: %s:%d\n", 
            ANSI_COLOR_RED, ANSI_COLOR_RESET, configs.address, configs.server_port);
}

int main(int argc, char *argv[]) {
    signal(SIGINT, quit);
    parse_arguments(argc, argv, &configs);
    green_client();
}