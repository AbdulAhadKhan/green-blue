#ifndef SERVER_BLUE_H
#define SERVER_BLUE_H

#include <getopt.h>

#include "meta.h"
#include "blue.h"

int server_blue(port_number_t port_number);
int server_blue_callback(void *args);

#endif