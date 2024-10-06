/* cache23.h */

#ifndef CACHE23
#define CACHE23

#define _GNU_SOURCE

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdarg.h>

#include <assert.h>
#include <errno.h>

#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define HOST    "127.0.0.1"
#define PORT    "12049"

// will only use comma if args is defined
//printf(x __VA_OPT__(,) __VA_ARGS__)
#define log(x, args ...) \
    printf(x, args); \
    fflush(stdout)

typedef unsigned int int32;
typedef unsigned short int int16;
typedef unsigned char int8;

int initserver(int16);
void mainloop(int);
int main(int, char**);

#endif
