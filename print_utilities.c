#include "print_utilities.h"

void print_message(const char * message)
{
    printf("%s\n", message);
}

void print_error_and_exit(const char * message)
{
    printf("%s\n", message);
    exit(2);
}

