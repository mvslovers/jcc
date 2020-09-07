#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <sockets.h>
/*#include "hercsock.h"*/

#define buffer_size 4096
#define default_port 25

//* Optional part...  If linking into another program, don't use this main ()...

long sendmail (char * smtp, char * from, char * user, char * subject, char * body);

void trim (char * s) {
    long i;

    i = strlen (s);
    while (--i >= 0) if (s [i] == ' ') s [i] = 0; else break;
};

int main () {
    long   i;
    long   j;
    long   l;
    FILE * fh;
    char   line1 [81];
    char   line2 [81];
    char   line3 [81];
    char   line4 [81];
    char   line5 [81];
    char * message;

    //Format of SYSIN: (FB80 and starting at line 1:)
    //192.168.0.1:25
    //jasonwinter@hotmail.com
    //jasonwinter@hotmail.com
    //This is a test
    //Testing 1... 2... 3...

    fh = fopen ("//DDN:SYSIN", "rb");
    if (fh) {

        i = fread (line1, 1, 80, fh);
        if (i != 80) {
            fclose (fh);
            printf ("No SMTP address:port.\n");
            return (1);
        };
        line1 [80] = 0;
        trim (line1);
        
        i = fread (line2, 1, 80, fh);
        if (i != 80) {
            fclose (fh);
            printf ("No send from email address.\n");
            return (1);
        };
        line2 [80] = 0;
        trim (line2);

        i = fread (line3, 1, 80, fh);
        if (i != 80) {
            fclose (fh);
            printf ("No send to email address.\n");
            return (1);
        };
        line3 [80] = 0;
        trim (line3);

        i = fread (line4, 1, 80, fh);
        if (i != 80) {
            fclose (fh);
            printf ("No subject.\n");
            return (1);
        };
        line4 [80] = 0;
        trim (line4);

        i = 4096;
        message = (char *)malloc (i);
        message [0] = 0;

        j = 0;
        while (fread (line5, 1, 80, fh) == 80) {
            
            line5 [80] = 0;
            trim (line5);

            l = strlen (line5);

            if (j + l + 3 >= i) {
                i = i * 2;
                message = (char *)realloc (message, i);
            };

            strcpy (&(message [j]), line5);
            j += l;
            strcpy (&(message [j]), "\r\n");
            j += 2;
        };

        fclose (fh);

        i = sendmail (line1, line2, line3, line4, message);

        free (message);

        return (i);

    } else {
        printf ("No SYSIN.\n");
    };

    return (1);
};

//*/ /* End optional part */

//eg. char smtp [260] = "192.168.0.1:25";           /* Which mail server to talk to */
//    char from [260] = "jasonwinter@hotmail.com";  /* User who has smtp account */
//    char user [260] = "jasonwinter@hotmail.com";  /* Who to send the message to */
//    char subject [260] = "This is a test";        /* Subject line */
//    char body [260] = "Testing 1... 2... 3...";   /* Message body, can include HTML */

//Return Value:
// >0 = Socket Error Number
//  0 = OK
// -1 = Bad Parameters (NULL or bad smtp address:port)
// -2 = Unexpected response from smtp server
// -3 = Unknown sender (from)
// -4 = Unknown recipient (user)

long sendmail (char * smtp, char * from, char * user, char * subject, char * body) {
    char * message;
    char * search;
    long   i;
    long   length;
    long   size;
    long   sockerr;
    unsigned short   port_number;
    char             ip_adx [260];
    SOCKET           Ccom_han;
    SOCKADDR_IN      Clocal_adx;
    struct hostent * result;
    char             buffer [buffer_size];

    if ((smtp == NULL) ||
        (from == NULL) ||
        (user == NULL) ||
        (subject == NULL) ||
        (body == NULL)) {

        printf ("input parameters failed.\n");
        return (-1);
    };

    // set up socket
    Clocal_adx.sin_family = AF_INET;
    strcpy (ip_adx, smtp);

    search = strstr (ip_adx, ":");
    if (search == NULL) {
        port_number = default_port;
    } else {
        search [0] = '\0';
        search++;
        port_number = atoi (search);
    };

    if ((Clocal_adx.sin_addr.s_addr = inet_addr (ip_adx)) == INADDR_NONE) {
        if ((result = gethostbyname (ip_adx)) == NULL) {
            printf ("gethostbyname failed.\n");
            return (-1);
        };
        if (result->h_addr_list[0] == NULL) {
            printf ("gethostbyname returned null.\n");
            return (-1);
        };
        Clocal_adx.sin_addr.s_addr = ((long *)(result->h_addr_list [0])) [0];
    };

    Clocal_adx.sin_port = ntohs (port_number);

    if ((Ccom_han = socket (PF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
        printf ("socket failed.\n");
        return (WSAGetLastError ());  // ERROR
    };

    if (connect (Ccom_han, (LPSOCKADDR)&Clocal_adx, sizeof (Clocal_adx))) {
        printf ("connect failed.\n");
        sockerr = WSAGetLastError ();
        closesocket (Ccom_han);
        return (sockerr);  // ERROR
    };

    // get command ready?
    if ((i = recv (Ccom_han, buffer, buffer_size, 0)) == SOCKET_ERROR) {
        printf ("recv 1 failed.\n");
        sockerr = WSAGetLastError ();
        closesocket (Ccom_han);
        return (sockerr);  // ERROR
    };
    ascii2ebcdic (buffer, i);
    if ((i < 3) || (strncmp (buffer, "220", 3) != 0)) {
        printf ("Initial-Wait Failed.\n");
        buffer [i] = 0;
        printf ("%s\n", buffer);
        closesocket (Ccom_han);
        return (-2);
    };

    sprintf (buffer, "helo 127.0.0.1\r\n");
    i = strlen (buffer);
    ebcdic2ascii (buffer, i);

    // transmit data
    if (send (Ccom_han, buffer, i, 0) == SOCKET_ERROR) {
        printf ("send 1 failed.\n");
        sockerr = WSAGetLastError ();
        closesocket (Ccom_han);
        return (sockerr);  // ERROR
    };

    // get command ready?
    if ((i = recv (Ccom_han, buffer, buffer_size, 0)) == SOCKET_ERROR) {
        printf ("recv 2 failed.\n");
        sockerr = WSAGetLastError ();
        closesocket (Ccom_han);
        return (sockerr);  // ERROR
    };
    ascii2ebcdic (buffer, i);
    if ((i < 3) || (strncmp (buffer, "250", 3) != 0)) {
        printf ("HELO Failed.\n");
        buffer [i] = 0;
        printf ("%s\n", buffer);
        closesocket (Ccom_han);
        return (-2);
    };

    sprintf (buffer, "MAIL FROM: <%s>\r\n", from);
    i = strlen (buffer);
    ebcdic2ascii (buffer, i);

    // transmit data
    if (send (Ccom_han, buffer, i, 0) == SOCKET_ERROR) {
        printf ("send 2 failed.\n");
        sockerr = WSAGetLastError ();
        closesocket (Ccom_han);
        return (sockerr);  // ERROR
    };

    // get command ready?
    if ((i = recv (Ccom_han, buffer, buffer_size, 0)) == SOCKET_ERROR) {
        printf ("recv 3 failed.\n");
        sockerr = WSAGetLastError ();
        closesocket (Ccom_han);
        return (sockerr);  // ERROR
    };
    ascii2ebcdic (buffer, i);
    if ((i < 3) || (strncmp (buffer, "250", 3) != 0)) {
        printf ("MAIL FROM: Failed.\n");
        buffer [i] = 0;
        printf ("%s\n", buffer);
        closesocket (Ccom_han);
        return (-3);
    };

    sprintf (buffer, "RCPT TO: <%s>\r\n", user);
    i = strlen (buffer);
    ebcdic2ascii (buffer, i);

    // transmit data
    if (send (Ccom_han, buffer, i, 0) == SOCKET_ERROR) {
        printf ("send 3 failed.\n");
        sockerr = WSAGetLastError ();
        closesocket (Ccom_han);
        return (sockerr);  // ERROR
    };

    // get command ready?
    if ((i = recv (Ccom_han, buffer, buffer_size, 0)) == SOCKET_ERROR) {
        printf ("recv 4 failed.\n");
        sockerr = WSAGetLastError ();
        closesocket (Ccom_han);
        return (sockerr);  // ERROR
    };
    ascii2ebcdic (buffer, i);
    if ((i < 3) || (strncmp (buffer, "250", 3) != 0)) {
        printf ("RCPT TO: Failed, check the correct email address.\n");
        buffer [i] = 0;
        printf ("%s\n", buffer);
        closesocket (Ccom_han);
        return (-4);
    };

    sprintf (buffer, "DATA\r\n");
    i = strlen (buffer);
    ebcdic2ascii (buffer, i);

    // transmit data
    if (send (Ccom_han, buffer, i, 0) == SOCKET_ERROR) {
        printf ("send 4 failed.\n");
        sockerr = WSAGetLastError ();
        closesocket (Ccom_han);
        return (sockerr);  // ERROR
    };

    // get command ready?
    if ((i = recv (Ccom_han, buffer, buffer_size, 0)) == SOCKET_ERROR) {
        printf ("recv 5 failed.\n");
        sockerr = WSAGetLastError ();
        closesocket (Ccom_han);
        return (sockerr);  // ERROR
    };
    ascii2ebcdic (buffer, i);
    if ((i < 3) || (strncmp (buffer, "354", 3) != 0)) {
        printf ("DATA Failed.\n");
        buffer [i] = 0;
        printf ("%s\n", buffer);
        closesocket (Ccom_han);
        return (-2);
    };

    i = strlen (body);
    message = malloc (i + buffer_size); /* Add a little extra space... */

    sprintf (message,

             "From: MVS38j <%s>\r\n"
             "To: <%s>\r\n"
             "Subject: %s\r\n"
             "Content-Type: text/html\r\n"
             "\r\n"
        
             "<HTML><BODY>\r\n"
             "%s"
             "</BODY></HTML>\r\n\r\n"

             ".\r\n",
             
             from, user, subject, body);

    length = strlen (message);
    ebcdic2ascii (message, length);

    i = 0;
    while (i < length) {
        size = length - i;
        if (size > buffer_size) size = buffer_size;

        // transmit data
        if (send (Ccom_han, &(message [i]), size, 0) == SOCKET_ERROR) {
            printf ("send 5 failed.\n");
            free (message);
            sockerr = WSAGetLastError ();
            closesocket (Ccom_han);
            return (sockerr);  // ERROR
        };

        i += size;
    };

    free (message);

    // get command ready?
    if ((i = recv (Ccom_han, buffer, buffer_size, 0)) == SOCKET_ERROR) {
        printf ("recv 6 failed.\n");
        sockerr = WSAGetLastError ();
        closesocket (Ccom_han);
        return (sockerr);  // ERROR
    };
    ascii2ebcdic (buffer, i);
    if ((i < 3) || (strncmp (buffer, "250", 3) != 0)) {
        printf ("Transmit Message Failed.\n");
        buffer [i] = 0;
        printf ("%s\n", buffer);
        closesocket (Ccom_han);
        return (-2);
    };

    sprintf (buffer, "QUIT\r\n");
    i = strlen (buffer);
    ebcdic2ascii (buffer, i);

    // transmit data
    if (send (Ccom_han, buffer, i, 0) == SOCKET_ERROR) {
        printf ("send 6 failed.\n");
        sockerr = WSAGetLastError ();
        closesocket (Ccom_han);
        return (sockerr);  // ERROR
    };

    // get command ready?
    if ((i = recv (Ccom_han, buffer, buffer_size, 0)) == SOCKET_ERROR) {
        printf ("recv 7 failed.\n");
        sockerr = WSAGetLastError ();
        closesocket (Ccom_han);
        return (sockerr);  // ERROR
    };
    ascii2ebcdic (buffer, i);
    if ((i < 3) || (strncmp (buffer, "221", 3) != 0)) {
        printf ("Quit Failed.\n");
        buffer [i] = 0;
        printf ("%s\n", buffer);
        closesocket (Ccom_han);
        return (-2);
    };

    // close connection
    if (closesocket (Ccom_han) == SOCKET_ERROR) {
        printf ("closesocket failed.\n");
        return (WSAGetLastError ());  // ERROR
    };

    return (0);
};
