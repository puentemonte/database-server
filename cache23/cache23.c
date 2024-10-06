/* cache23.c */

#include "cache23.h"

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

bool scontinuation; // server continuation
bool ccontinuation; // client continuation
                    
void zero(int8* buf, int16 size) {
    int8 *p;
    int16 n;

    for(n=0, p=buf; n<size; ++n, ++p) {
        *p = 0;
    }
}

void childloop(Client *client) {
    sleep(1);

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

    printf("Connection from %s:%d\n", ip, port);

    client = (Client *)malloc(sizeof(struct s_client));
    assert(client);

    zero((int8 *) client, sizeof(struct s_client));
    client->s = s;
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
