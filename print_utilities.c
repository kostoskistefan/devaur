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

void print_usage()
{
    fprintf(stderr, "Usage: devaur [-SRC] package_name\n");
    exit(EXIT_FAILURE);
}
