#include <string.h>
#include <stdlib.h>
#include <sockets.h>

#define buffer_size 4096
#define default_port 21
#define ftp "127.0.0.1" // Assume this machine

long sendcmd (long term) {
    unsigned short   port_number;
    char             ip_adx [260];
    SOCKET           Ccom_han;
    SOCKADDR_IN      Clocal_adx;
    char             buffer [buffer_size];
    char           * search;
    struct hostent * result;
    int              sockerr;
    int              i;

    // set up socket
    Clocal_adx.sin_family = AF_INET;
    strcpy (ip_adx, ftp);
    search = strstr (ip_adx, ":");
    if (search == NULL) {
        port_number = default_port;
    } else {
        search [0] = '\0';
        search++;
        port_number = atoi (search);
    }

    if ((Clocal_adx.sin_addr.s_addr = inet_addr (ip_adx)) == INADDR_NONE) {
        if ((result = gethostbyname (ip_adx)) == NULL) {
            printf ("gethostbyname failed.\n");
            return (-1);
        }
        if (result->h_addr_list[0] == NULL) {
            printf ("gethostbyname returned null.\n");
            return (-1);
        }
        Clocal_adx.sin_addr.s_addr = ((long *)(result->h_addr_list [0])) [0];
    }

    Clocal_adx.sin_port = ntohs (port_number);

    if ((Ccom_han = socket (PF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
        printf ("socket failed.\n");
        return (WSAGetLastError ());  // ERROR
    }

    if (connect (Ccom_han, (LPSOCKADDR)&Clocal_adx, sizeof (Clocal_adx))) {
        printf ("connect failed.\n");
        sockerr = WSAGetLastError ();
        closesocket (Ccom_han);
        return (sockerr);  // ERROR
    }

    // get command ready?
    if ((i = recv (Ccom_han, buffer, buffer_size, 0)) == SOCKET_ERROR) {
        printf ("recv 1 failed.\n");
        sockerr = WSAGetLastError ();
        closesocket (Ccom_han);
        return (sockerr);  // ERROR
    }
    ascii2ebcdic (buffer, i);
    if ((i < 3) || (strncmp (buffer, "220", 3) != 0)) {
        printf ("Initial-Wait Failed.\n");
        buffer [i] = 0;
        printf ("%s\n", buffer);
        closesocket (Ccom_han);
        return (-2);
    }

    if (term)
        sprintf (buffer, "term\r\n"); // Terminate command for MVS3.8j FTPD
    else
        sprintf (buffer, "refr\r\n"); // Refresh command for MVS3.8j FTPD
    i = strlen (buffer);
    ebcdic2ascii (buffer, i);

    // transmit data
    if (send (Ccom_han, buffer, i, 0) == SOCKET_ERROR) {
        printf ("send 1 failed.\n");
        sockerr = WSAGetLastError ();
        closesocket (Ccom_han);
        return (sockerr);  // ERROR
    }

    // get command ready?
    if ((i = recv (Ccom_han, buffer, buffer_size, 0)) == SOCKET_ERROR) {
        printf ("recv 2 failed.\n");
        sockerr = WSAGetLastError ();
        closesocket (Ccom_han);
        return (sockerr);  // ERROR
    }

    ascii2ebcdic (buffer, i);
    buffer [i] = 0;
    printf ("%s\n", buffer);

    sprintf (buffer, "quit\r\n");
    i = strlen (buffer);
    ebcdic2ascii (buffer, i);

    // transmit data
    if (send (Ccom_han, buffer, i, 0) == SOCKET_ERROR) {
        printf ("send 2 failed.\n");
        sockerr = WSAGetLastError ();
        closesocket (Ccom_han);
        return (sockerr);  // ERROR
    }

    // close connection
    if (closesocket (Ccom_han) == SOCKET_ERROR) {
        printf ("closesocket failed.\n");
        return (WSAGetLastError ());  // ERROR
    }

    return (0);
}

int main (int argc, char ** argv) {
    if (argc == 1) // No command - assume refresh
        return (sendcmd (0));
    else          // Otherwise - terminate FTPD
        return (sendcmd (1));
}
