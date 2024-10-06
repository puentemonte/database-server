/* cache23.c */

#include "cache23.h"


bool scontinuation; // server continuation

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

void mainloop(int s) {

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

    return 0;
}
