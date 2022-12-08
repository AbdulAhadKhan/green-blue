#include <string.h>
#include <sys/wait.h>

#include "utils/meta.h"
#include "utils/server-blue.h"

struct server {
    port_number_t port_number;
    socket_fd_t socket;
    int number_of_clients;
};

struct config {
    port_number_t load_balancer_port_number;
    port_number_t starting_port_number;
    int number_of_servers;
    struct server *servers;
};

static struct option long_options[] = {
    {"captain-port",        required_argument,  0,  'p'},
    {"server-port",         required_argument,  0,  's'},
    {"number-of-servers",   required_argument,  0,  'n'},
    {0,                     0,                  0,  0}
};

int parse_arguments(int argc, char *argv[], struct config *config) {
    int opt;

    while ((opt = getopt_long(argc, argv, "p:s:n:", long_options, NULL)) != -1) {
        switch (opt) {
            case 'p': config->load_balancer_port_number = atoi(optarg); break;
            case 's': config->starting_port_number = atoi(optarg); break;
            case 'n': config->number_of_servers = atoi(optarg); break;
            default: fprintf(stderr, "Usage: %s [-p port] [-s port] [-n number]\n", argv[0]); return -1;
        }
    }
    
    return 0;
}

int forward_request(socket_fd_t *server_socket, port_number_t port_number, char *request, int *number_of_clients) {
    char message[1024];
    socket_fd_t client_socket = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in server_address = socket_object_init(port_number);
    (*number_of_clients)++;
    printf("Number of clients: %d\n", *number_of_clients);
    connect(client_socket, (struct sockaddr *) &server_address, sizeof(server_address));
    send(client_socket, request, strlen(request), 0);
    (*number_of_clients)--;
    printf("Number of clients: %d\n", *number_of_clients);
    return 0;
}

int captain_callback(void *args) {
    struct config *config = (struct config *) args;
    printf("Captain Callback\n");

    for (int i = 0; i < config->number_of_servers; i++) {
        if (config->servers[i].number_of_clients < 5) {
            printf("Forwarding request to server %d\n", i);
            forward_request(&config->servers[i].socket, config->servers[i].port_number, \
                            "Hello", &config->servers[i].number_of_clients);
            break;
        }
    }
    
    return 0;
}

int server(struct config *config) {
    socket_fd_t socket_fd = initialize_server(config->load_balancer_port_number);
    return socket_fd < 0 || start_server(socket_fd, captain_callback, config) < 0 ? -1 : 0;
}

int captain_balancer(struct config *config) {
    pid_t pid;

    if ((pid = fork()) == 0)
        return server(config);
    
    for (int i = 0; i < config->number_of_servers; i++) {
        config->servers[i].port_number = config->starting_port_number + i;
        config->servers[i].socket = initialize_server(config->servers[i].port_number);
        config->servers[i].number_of_clients = 0;
        if ((pid = fork()) == 0)
            return config->servers[i].socket < 0 || \
                   start_server(config->servers[i].socket, NULL, NULL) < 0 ? -1 : 0;
    }
    
    waitpid(pid, NULL, 0);
    return 0;
}

int main(int argc, char *argv[]) {
    struct config config = { 2000, 3000, 1, NULL };
    parse_arguments(argc, argv, &config);
    config.servers = (struct server *) malloc(config.number_of_servers * sizeof(struct server));
    captain_balancer(&config);
    free(config.servers);
}