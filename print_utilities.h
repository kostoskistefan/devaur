#ifndef PRINT_UTILITIES_H
#define PRINT_UTILITIES_H

#include <stdio.h>
#include <stdlib.h>

extern void print_message(const char * message);
extern void print_error_and_exit(const char * message, int status_code);
extern void print_usage();

#endif
