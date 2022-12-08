#include <string.h>
#include <sys/wait.h>
#include <sys/mman.h>

#include "utils/meta.h"
#include "utils/server-blue.h"

struct server {
    port_number_t port_number;
    socket_fd_t socket;
    int *shared_connection_count;
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

struct config * create_shared_memory() {
    int protection = PROT_READ | PROT_WRITE;
    int visibility = MAP_SHARED | MAP_ANONYMOUS;
    return mmap(NULL, sizeof(struct config), protection, visibility, -1, 0);
}

int start_blue_servers(struct config *config) {
    printf("Starting %d server blues\n", config->number_of_servers);

    for (int i = 0; i < config->number_of_servers; i++) {
        config->servers[i].port_number = config->starting_port_number + i;
        config->servers[i].socket = initialize_server(config->servers[i].port_number);
        config->servers[i].shared_connection_count = 0;
        if (fork() == 0)
            return config->servers[i].socket < 0 || \
                   start_server(config->servers[i].socket, NULL, NULL) < 0 ? -1 : 0;
    }

    perror("Server Blue");

    return 0;
}

int forward_request(socket_fd_t *server_socket, port_number_t port_number, char *request, int *shared_connection_count) {
    printf("Forwarding request to server on port %d\n", port_number);

    return 0;
}

int below_threshold(struct config *config, int threshold) {
    int connection_count;
    for (int i = 0; i < config->number_of_servers; i++)
        if (*((int *) config->servers[i].shared_connection_count) < threshold)
            return i;
    return -1;
}

int round_robin(struct config *config) {
    int lowest_load = 0;
    int lowest_load_index = 0;
    int current_load;

    for (int i = 0; i < config->number_of_servers; i++) {
        current_load = *((int *) config->servers[i].shared_connection_count);
        if (current_load < lowest_load) {
            lowest_load = current_load;
            lowest_load_index = i;
        }
    }

    return lowest_load_index;
}

int captains_callback(void *args) {
    char client_message[1024];
    int elected_server;

    if (((struct config *) args)->number_of_servers <= 0) {
        send(client_connection, "No servers available", 20, 0);
        return -1;
    }

    // if ((elected_server = below_threshold(config, 5)) < 0)
    //     elected_server = round_robin(config);
    
    port_number_t elected_port = ((struct config *) args)->servers[0].port_number;

    printf("Elected server port: %d\n", elected_port);

    // forward_request(&config->servers[elected_server].socket, \
    //                 config->servers[elected_server].port_number, \
    //                 client_message, config->servers[elected_server].shared_connection_count);

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
    struct config *config = create_shared_memory();
    *config = (struct config) { 2000, 3000, 1, NULL };
    parse_arguments(argc, argv, config);

    int protection = PROT_READ | PROT_WRITE;
    int visibility = MAP_SHARED | MAP_ANONYMOUS;
    struct server *servers = mmap(NULL, sizeof(struct server) * config->number_of_servers, protection, visibility, -1, 0);
    
    config->servers = servers;
    captain_balancer(config);
}