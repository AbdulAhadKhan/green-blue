#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>

#include <pthread.h>
#include <sys/mman.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "utils/meta.h"
#include "utils/ANSI-colors.h"

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

int green_client() {
    char message[MESSAGE_SIZE];
    char buffer[MESSAGE_SIZE];
    int read_size;
    int network_socket = socket(AF_INET, SOCK_STREAM, 0);
    int protection = PROT_READ | PROT_WRITE;
    int visibility = MAP_SHARED | MAP_ANONYMOUS;

    pthread_t thread;
    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = inet_addr(configs.address);
    server_address.sin_port = htons(configs.server_port);
    server_address.sin_addr.s_addr = INADDR_ANY;

    if (connect(network_socket, (struct sockaddr *) &server_address, sizeof(server_address)) != -1) {
        pthread_create(&thread, NULL, check_connection_thread, &network_socket);
        printf("%s[+]%s Connected to server: %s:%d\n", ANSI_COLOR_GREEN, ANSI_COLOR_RESET, configs.address, configs.server_port);
        while (1) {
            printf("Message: ");
            fgets(message, sizeof(message), stdin);
            send(network_socket, message, sizeof(message), 0);
            read_size = recv(network_socket, &buffer, sizeof(buffer), 0);
            buffer[read_size] = '\0';
            printf("Server response: %s\n", buffer);
        }
    }

    fprintf(stderr, "%s[-]%s Failed to connect to server: %s:%d\n", 
            ANSI_COLOR_RED, ANSI_COLOR_RESET, configs.address, configs.server_port);

    return -1;
}

int main(int argc, char *argv[]) {
    parse_arguments(argc, argv, &configs);
    green_client();
}