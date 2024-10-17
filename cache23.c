/* cache23.c */

#include "cache23.h"

bool scontinuation; // server continuation
bool ccontinuation; // client continuation

// Size of handlers is 16 * #handlers because both cmd and callback are 8 byte pointers
CmdHandler handlers[] = {
    { (int8 *)"hello", handle_hello },
    { (int8 *)"goodbye", handle_hello }
};

Callback getcmd(int8 *cmd) {
    Callback cb;
    int16 n, arrlen;
    
    if(sizeof(handlers) < 16) {
        return 0;
    }
    arrlen = sizeof(handlers) / 16;

    cb = 0;

    for(n=0; n<arrlen; ++n) {
        if(!strcmp((char *)cmd, (char *)handlers[n].cmd)) {
            cb = handlers[n].handler;
        }
    }

    return cb;
}

int32 handle_hello(Client *client,
                   int8 *dir,
                   int8 *args) {
   dprintf(client->s, "hello %s\n", dir);
   return 0;
}

int initserver(int16 port) {
    struct sockaddr_in sock;
    int s;
    
    sock.sin_family = AF_INET;
    sock.sin_port = htons(port);
    sock.sin_addr.s_addr = inet_addr(HOST);

    s = socket(AF_INET, SOCK_STREAM, 0);
    assert(s>0);

    errno = 0;
    if(bind(s, (struct sockaddr *)&sock, sizeof(sock))) {
        assert_perror(errno);
    }

    errno = 0;
    if(listen(s, 20)) {
        assert_perror(errno);
    }

   log("Server listening on %s:%d", HOST, port);
    
    return s;
}

void zero(int8* buf, int16 size) {
    int8 *p;
    int16 n;

    for(n=0, p=buf; n<size; ++n, ++p) {
        *p = 0;
    }
    return;
}

 // command ie.: select /Users/erik
 //              create /Users/logins
 //              insert /Users/erik data...

void childloop(Client *client) {
    int8 buf[256]; 
    int8 *p, *f;  
    int16 n;  
    int8 cmd[256], dir[256], args[256];

    zero(buf, 256);
    read(client->s, (char *)buf, 255);
    n = (int16)strlen((char *)buf);

    if(n > 254) {
        n = 254;
    }

    for(p=buf;
        (*p) &&
        (n--) &&
        (*p != ' ') &&
        (*p != '\n') &&
        (*p != '\r');
        ++p);

    zero(cmd, 256);
    zero(dir, 256);
    zero(args, 256);


    if(!(*p) || (!n)) {
        strncpy((char *)cmd, (char *)buf, 255);
        goto end;
    }
    else if(*p == ' ') {
        *p = 0;
        strncpy((char *)cmd, (char *)buf, 255);
    }
    else if((*p == '\n') ||
            (*p == '\r')) {
        *p = 0;
        strncpy((char *)cmd, (char *)buf, 255);
        goto end;
    }

    for(f=++p;
            (*p) &&
            (n--) &&
            (*p != ' ') &&
            (*p != '\n') &&
            (*p != '\r');
         ++p);

    if(!(*p) || (!n)) {
        strncpy((char *)dir, (char *)f, 255);
        goto end;
    }
    else if((*p == ' ') ||
            (*p == '\n') ||
            (*p == '\r')) {
        *p = 0;
        strncpy((char *)dir, (char *)f, 255);
    }

    ++p;
    if(*p) {
        strncpy((char *)args, (char *)p, 255);
        for(p=args; (*p) && 
                    (*p != '\n') && 
                    (*p != '\r');++p);
        *p = 0;
    }
    

end:
     dprintf(client->s, "\ncmd:\t%s\n", cmd);
     dprintf(client->s, "dir:\t%s\n", dir);
     dprintf(client->s, "args:\t%s\n", args);

    return;
}

void mainloop(int s) {
    struct sockaddr_in cli;
    int32 len;
    int sclient;
    char* ip;
    int16 port;
    Client *client;
    pid_t pid;

    sclient = accept(s, (struct sockaddr *)&cli, (unsigned int *)&len);
    if(sclient < 0) {
        return;
    }

    port = (int16)htons((int)cli.sin_port);
    ip = inet_ntoa(cli.sin_addr);

    printf("\nConnection from %s:%d\n", ip, port);

    client = (Client *)malloc(sizeof(struct s_client));
    assert(client);

    zero((int8 *)client, sizeof(struct s_client));
    client->s = sclient;
    client->port = port;
    strncpy(client->ip, ip, 15);

    pid = fork();
    if(pid) { // we are in the parent
        free(client);
        return;
    }
    else {
        dprintf(sclient, "100 Connected to Cache23 server\n");

        ccontinuation = true;
        while(ccontinuation) {
            childloop(client);
        }
        close(sclient);
        free(client);

        return;
    }

    return;
}

int main(int argc, char** argv) {
    char *sport;
    int16 port;
    int s;

    if(argc < 2) {
        sport = PORT;
    }
    else {
       sport = argv[1]; 
    }
    port = (int16)atoi(sport);

    scontinuation = true;
    s = initserver(port);
    while(scontinuation) {
        mainloop(s);
    }
    
    log("Shutting down server in port %d\n", port);
    close(s);

    return 0;
}
#pragma GCC diagnostic pop
