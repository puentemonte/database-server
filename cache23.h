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

#pragma GCC diagnostic ignored "-Wunused-result"
#pragma GCC diagnostic ignored "-Wstringop-truncation"
#pragma GCC diagnostic push

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

struct s_client {
    int s;
    char ip[16]; // 255.255.255.255\0 = 16 bytes
     int16 port;
};

typedef struct s_client Client;

// Callback for the client commands: dir args
typedef int32 (*Callback)(Client*,int8*,int8*);

struct s_cmdhandler {
    int8* cmd;
    Callback handler;
};

typedef struct s_cmdhandler CmdHandler;

void zero(int8*,int16);
int initserver(int16);
void mainloop(int);
void childloop(Client*);
int main(int,char**);

int32 handle_hello(Client*,int8*,int8*);

#endif
