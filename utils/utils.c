#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "utils.h"

/**
 * @brief Get the current time as string
 * 
 * @return char* 
 */
char * get_current_time_as_string() {
    time_t current_time;
    char *date_time_string = malloc(100);

    time(&current_time);
    strftime(date_time_string, 100, "%Y-%m-%d %H:%M:%S", localtime(&current_time));

    return date_time_string;
}