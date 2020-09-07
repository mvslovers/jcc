#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

int main (int argc, char * argv []) {
    FILE * fh;
    FILE * fho;
    char   num [260];
    char   buf [260];
    char * a;
    char * b;
    long   i;

    if (argc != 3) {
        printf ("Usage: dbgscan input.lst output.tst\n");
        printf (" Appends assembler output to a test file created by JCC.\n");
        return (8);
    };

    fh = fopen (argv [1], "r");
    if (fh) {

        fho = fopen (argv [2], "a"); /* Always append to JCC's test output file */
        if (fho == NULL) {
            fclose (fh);
            printf ("Can't open the JCC test file for append.\n");
            return (8);
        };

        /* Find CSECT name first */
        while (fgets (num, 260, fh)) {

            a = strstr (num, "CSECT");
            if (a) {
                a--;
                if (*a == ' ') {
                    while (*a == ' ') a--;
                    a [1] = 0;
                    while (*a != ' ') a--;
                    a++;
                    fprintf (fho, "&Object: %s\n", a);
                    break;
                } else
                if (*a == '_') {
                    a += 8;
                    fprintf (fho, "&Object: %s", a); /* Has newline! */
                    break;
                };
            };
            a = strstr (num, "ALIAS");
            if (a) {
                a = strstr (a, "'");
                if (a) {
                    a++;
                    b = strstr (a, "'");
                    if (b) {
                        b [0] = 0;
                        fprintf (fho, "&Object: %s\n", a);
                        break;
                    };
                };
            };
        };

        /* Now find addresses of line numbers */
        i = 0;
        while (fgets (num, 260, fh)) {

            if ((i) && (isdigit (num [1]))) {

                i = strtol (&(num [1]), NULL, 16);
                fprintf (fho, "%08X %s", i, buf); /* Has newline! */
                i = 0;
            };

            if ((a = strstr (num, "* *** ")) != NULL) {
                strcpy (buf, &(a [15]));
                i = 1;
            };
        };

        fclose (fh);
        fclose (fho);

        return (0);
    } else {
        printf ("Can't open file %s.\n", argv [1]);
    };

    return (8);
};
