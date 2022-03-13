#include "print_utilities.h"

void print_message(const char * message)
{
    printf("%s\n", message);
}

void print_error_and_exit(const char * message, int status_code)
{
    printf("%s, Status code: %d\n", message, status_code);
    exit(2);
}

void print_usage(char * app_name)
{
    fprintf(stderr, "Usage: %s [-SRC] package_name\n", app_name);
    exit(EXIT_FAILURE);
}
