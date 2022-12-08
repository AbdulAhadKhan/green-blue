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

int start_blue_servers(struct config *config) {
    printf("Starting %d server blues\n", config->number_of_servers);

    for (int i = 0; i < config->number_of_servers; i++) {
        config->servers[i].port_number = config->starting_port_number + i;
        config->servers[i].socket = initialize_server(config->servers[i].port_number);
        config->servers[i].number_of_clients = 0;
        if (fork() == 0)
            return config->servers[i].socket < 0 || \
                   start_server(config->servers[i].socket, NULL, NULL) < 0 ? -1 : 0;
    }

    return 0;
}

int forward_request(socket_fd_t *server_socket, port_number_t port_number, char *request, int *number_of_clients) {
    (*number_of_clients)++;
    while (recv(client_connection, NULL, 1, MSG_PEEK | MSG_DONTWAIT ) != 0) {
        printf("Number of clients: %d\n", *number_of_clients);
        sleep(30);
    }

    (*number_of_clients)--;
    printf("Number of clients: %d\n", *number_of_clients);
    return 0;
}

int below_threshold(struct config *config, int threshold) {
    for (int i = 0; i < config->number_of_servers; i++)
        if (config->servers[i].number_of_clients < threshold)
            return i;
    return -1;
}

int round_robin(struct config *config) {
    int lowest_load = 0;
    int lowest_load_index = 0;

    for (int i = 0; i < config->number_of_servers; i++) {
        if (config->servers[i].number_of_clients < lowest_load) {
            lowest_load = config->servers[i].number_of_clients;
            lowest_load_index = i;
        }
    }

    return lowest_load_index;
}

int captains_callback(void *args) {
    char client_message[1024];
    struct config *config = (struct config *) args;
    int elected_server;

    if ((elected_server = below_threshold(config, 5)) < 0)
        elected_server = round_robin(config);

    forward_request(&config->servers[elected_server].socket, \
                    config->servers[elected_server].port_number, \
                    client_message, &config->servers[elected_server].number_of_clients);

     return -1;
}

int server(struct config *config) {
    socket_fd_t socket_fd = initialize_server(config->load_balancer_port_number);
    return socket_fd < 0 || start_server(socket_fd, captains_callback, config) < 0 ? -1 : 0;
}

int captain_balancer(struct config *config) {
    pid_t pid;
    if ((pid = fork()) == 0)
        return server(config);
    start_blue_servers(config);
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