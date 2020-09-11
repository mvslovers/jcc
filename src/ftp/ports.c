#include <stdio.h>

#include <sockets.h>
/*#include "hercsock.h"*/

void main () {
    SOCKET i;
    struct herc_in_addr bacc;
    struct sockaddr a;
    struct sockaddr_in * b;
    int l = sizeof (a);

    i = 1;
    while (i != 1024) {

        b = (void *)&a;
        if (getsockname (i, &a, &l) != SOCKET_ERROR) {

            bacc.S_un.S_addr = b->sin_addr.s_addr;

            printf ("Socket %d connected to %d.%d.%d.%d:%d\n", i,
                bacc.S_un.S_un_b.s_b1,
                bacc.S_un.S_un_b.s_b2,
                bacc.S_un.S_un_b.s_b3,
                bacc.S_un.S_un_b.s_b4,
                ntohs (b->sin_port));
        };

        i++;
    };
};
