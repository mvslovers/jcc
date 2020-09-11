#include <sockets.h>
/*#include "hercsock.h"*/

void main () {
    long i;

    i = 1;
    while (i != 1024) {
        closesocket (i);
        i++;
    };
};
