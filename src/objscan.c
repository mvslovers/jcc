/*
*    Copyright (c) 2003, Jason Paul Winter - All Rights Reserved     *
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef WIN32
#ifndef JCC
#define stricmp strcasecmp
#endif
#endif

typedef struct sym_tag * sym_ptr;
typedef struct sym_tag {
    char  * sname;     /* Short Name */
    char  * name;      /* Long Name */
    sym_ptr next;
} sym_t;

sym_ptr shead = NULL;

char esdrec [3];
char xsdrec [3];

#ifndef JCC

unsigned char __ascii_to_ebcdic[] = {
    "\x00\x01\x02\x03\x37\x2D\x2E\x2F\x16\x05\x15\x0B\x0C\x0D\x0E\x0F\x10\x11\x12\x13\x3C\x3D\x32\x26\x18\x19\x3F\x27\x1C\x1D\x1E\x1F"
    "\x40\x5A\x7F\x7B\x5B\x6C\x50\x7D\x4D\x5D\x5C\x4E\x6B\x60\x4B\x61\xF0\xF1\xF2\xF3\xF4\xF5\xF6\xF7\xF8\xF9\x7A\x5E\x4C\x7E\x6E\x6F"
    "\x7C\xC1\xC2\xC3\xC4\xC5\xC6\xC7\xC8\xC9\xD1\xD2\xD3\xD4\xD5\xD6\xD7\xD8\xD9\xE2\xE3\xE4\xE5\xE6\xE7\xE8\xE9\xAD\xE0\xBD\x5F\x6D"
    "\x79\x81\x82\x83\x84\x85\x86\x87\x88\x89\x91\x92\x93\x94\x95\x96\x97\x98\x99\xA2\xA3\xA4\xA5\xA6\xA7\xA8\xA9\xC0\x4F\xD0\xA1\x07"
    "\x20\x21\x22\x23\x24\x25\x06\x17\x28\x29\x2A\x2B\x2C\x09\x0A\x1B\x30\x31\x1A\x33\x34\x35\x36\x08\x38\x39\x3A\x3B\x04\x14\x3E\xFF"
    "\x41\xAA\x4A\xB1\x9F\xB2\x6A\xB5\xBB\xB4\x9A\x8A\xB0\xCA\xAF\xBC\x90\x8F\xEA\xFA\xBE\xA0\xB6\xB3\x9D\xDA\x9B\x8B\xB7\xB8\xB9\xAB"
    "\x64\x65\x62\x66\x63\x67\x9E\x68\x74\x71\x72\x73\x78\x75\x76\x77\xAC\x69\xED\xEE\xEB\xEF\xEC\xBF\x80\xFD\xFE\xFB\xFC\xBA\xAE\x59"
    "\x44\x45\x42\x46\x43\x47\x9C\x48\x54\x51\x52\x53\x58\x55\x56\x57\x8C\x49\xCD\xCE\xCB\xCF\xCC\xE1\x70\xDD\xDE\xDB\xDC\x8D\x8E\xDF"
};

unsigned char __ebcdic_to_ascii[] = {
    "\x00\x01\x02\x03\x9C\x09\x86\x7F\x97\x8D\x8E\x0B\x0C\x0D\x0E\x0F\x10\x11\x12\x13\x9D\x0A\x08\x87\x18\x19\x92\x8F\x1C\x1D\x1E\x1F"
    "\x80\x81\x82\x83\x84\x85\x17\x1B\x88\x89\x8A\x8B\x8C\x05\x06\x07\x90\x91\x16\x93\x94\x95\x96\x04\x98\x99\x9A\x9B\x14\x15\x9E\x1A"
    "\x20\xA0\xE2\xE4\xE0\xE1\xE3\xE5\xE7\xF1\xA2\x2E\x3C\x28\x2B\x7C\x26\xE9\xEA\xEB\xE8\xED\xEE\xEF\xEC\xDF\x21\x24\x2A\x29\x3B\x5E"
    "\x2D\x2F\xC2\xC4\xC0\xC1\xC3\xC5\xC7\xD1\xA6\x2C\x25\x5F\x3E\x3F\xF8\xC9\xCA\xCB\xC8\xCD\xCE\xCF\xCC\x60\x3A\x23\x40\x27\x3D\x22"
    "\xD8\x61\x62\x63\x64\x65\x66\x67\x68\x69\xAB\xBB\xF0\xFD\xFE\xB1\xB0\x6A\x6B\x6C\x6D\x6E\x6F\x70\x71\x72\xAA\xBA\xE6\xB8\xC6\xA4"
    "\xB5\x7E\x73\x74\x75\x76\x77\x78\x79\x7A\xA1\xBF\xD0\x5B\xDE\xAE\xAC\xA3\xA5\xB7\xA9\xA7\xB6\xBC\xBD\xBE\xDD\xA8\xAF\x5D\xB4\xD7"
    "\x7B\x41\x42\x43\x44\x45\x46\x47\x48\x49\xAD\xF4\xF6\xF2\xF3\xF5\x7D\x4A\x4B\x4C\x4D\x4E\x4F\x50\x51\x52\xB9\xFB\xFC\xF9\xFA\xFF"
    "\x5C\xF7\x53\x54\x55\x56\x57\x58\x59\x5A\xB2\xD4\xD6\xD2\xD3\xD5\x30\x31\x32\x33\x34\x35\x36\x37\x38\x39\xB3\xDB\xDC\xD9\xDA\x9F"
};

#define byteswap(i) (((i & 0x00FF) << 8) | ((i & 0xFF00) >> 8))
#define wordswap(i) (((i & 0x000000FF) << 24) | ((i & 0xFF000000) >> 24) | ((i & 0x0000FF00) << 8) | ((i & 0x00FF0000) >> 8))

void atoe (char * d, long len) {
    long i;

    i = 0;
    while (i < len) {
        d [i] = __ascii_to_ebcdic [((unsigned char *)d) [i]];
        i++;
    };
};

void etoa (char * d, long len) {
    long i;

    i = 0;
    while (i < len) {
        d [i] = __ebcdic_to_ascii [((unsigned char *)d) [i]];
        i++;
    };
};

#else

#define byteswap(i) (i)
#define wordswap(i) (i)

void atoe (char * d, long len) {
};

void etoa (char * d, long len) {
};

#endif

int copy (char * file, FILE * fho) {
    unsigned char buf1 [81];
    unsigned char buf2 [81];
    char    name [41];
    FILE  * fh;
    long    i;
    long    j;
    long    k;
    sym_ptr sym;
    short   esdid;
    short   ldid = 1;

    printf ("Building from <%s>\n", file);

    fh = fopen (file, "rb");
    if (fh != NULL) {
        while (fread (buf1, 1, 80, fh) == 80) {

            if (buf1 [0] == 2) { /* Object Record */

                if (strncmp (esdrec, &(buf1 [1]), 3) == 0) { /* ESD */

                    esdid = byteswap (*((short *)(&buf1 [14]))); /* First ESDID */

                    i = byteswap (*((short *)(&buf1 [10])));
                    j = 16;
                    while (i > 0) { /* variable number of bytes in record */

                        memcpy (name, &(buf1 [j]), 8); /* Get Name */
                        etoa (name, 8);
                        k = 8;
                        while ((k) && (name [k-1] == ' ')) k--;
                        name [k] = 0;

                        sym = shead;
                        while (sym) {
                            if (strcmp (sym->sname, name) == 0) break;
                            sym = sym->next;
                        };
                        if (sym == NULL) {
                            fprintf (stderr, "Name <%s> not translated.\n", name);
                        };

                        buf2 [0] = 2;
                        memcpy (&(buf2 [1]), xsdrec, 3);
                        memset (&(buf2 [4]), 0x40, 6);          /* Blank the record */
                        memset (&(buf2 [10]), 0x00, 22);        /* Zero the record */
                        memset (&(buf2 [32]), 0x40, 48);        /* Blank the record */
                        //memcpy (&(buf2 [72]), &(buf1 [72]), 8); /* Copy same line number 3 times */

                        if (buf1 [j + 8] != 1) { /* LD's don't have an ESDID */
                            ((short *)(&buf2 [14])) [0] = byteswap (esdid);
                            esdid++;
                        } else {
                            ((short *)(&buf2 [14])) [0] = byteswap (ldid);
                            ldid++;
                        };

                        if (sym == NULL)
                            k = strlen (name);
                        else
                            k = strlen (sym->name);

                        buf2 [11] = k + 0x10;

                        ((long *)(&buf2 [16])) [0] = wordswap (k); /* Name Length */

                        buf2 [23] = 1; /* They all seem to have this? */
                        memcpy (&(buf2 [24]), &(buf1 [j + 8]), 8); /* ESDType, Addr, Flag, Len/LDID */

                        if (buf2 [24] == 0x00) { /* Type=SD, has flags which we may modify here */
                            buf2 [28] =  0x07;   /* rmode=any, amode=any */
                        };

                        if (sym == NULL)
                            memcpy (&(buf2 [32]), name, k);
                        else
                            memcpy (&(buf2 [32]), sym->name, k);

                        atoe (&(buf2 [32]), k);

                        j += 16;
                        i -= 16;

                        fwrite (buf2, 1, 80, fho);
                    };

                } else if (strncmp (xsdrec, &(buf1 [1]), 3) == 0) { /* XSD */

                    memcpy (buf2, buf1, 80);
                    etoa (buf2, 80);

                    i = wordswap (*((long *)(&buf1 [16]))); /* Name Length */
                    strncpy (name, &(buf2 [32]), i); /* Name */
                    name [i] = 0;

                    sym = shead;
                    while (sym) {
                        if (strcmp (sym->sname, name) == 0) break;
                        sym = sym->next;
                    };
                    if (sym == NULL) {
                        fprintf (stderr, "Name <%s> not translated.\n", name);
                    };

                    memset (&(buf1 [32]), 0x40, i);

                    if (sym == NULL)
                        j = strlen (name);
                    else
                        j = strlen (sym->name);

                    buf1 [11] = j + 0x10;
                    ((long *)(&buf1 [16])) [0] = wordswap (j); /* Name Length */

                    if (sym == NULL)
                        memcpy (&(buf1 [32]), name, j);
                    else
                        memcpy (&(buf1 [32]), sym->name, j);

                    atoe (&(buf1 [32]), j);

                    if (buf1 [24] == 0x00) { /* Type=SD, has flags which we may modify here */
                        buf1 [28] =  0x07;   /* rmode=any, amode=any */
                    };

                    fwrite (buf1, 1, 80, fho);
                } else {
                    fwrite (buf1, 1, 80, fho);
                };
            } else {
                fwrite (buf1, 1, 80, fho);
            };
        };
        fclose (fh);
        return (0);
    };

    fprintf (stderr, "File <%s> not found.\n", file);

    return (1);
};

int main (int argc, char * argv []) {
    long    i;
    char  * a;
    FILE  * fh;
    FILE  * fho;
    char    buf [260];
    sym_ptr s;

    if (argc != 4) {
        fprintf (stderr, "Usage: objscan input.obj input.nam output.obj\n");
        fprintf (stderr, " Takes a pre-built name file and converts ESDs to XSDs\n");
        return (1);
    };

    fh = fopen (argv [2], "r");
    if (fh == NULL) {
        fprintf (stderr, "The input.nam file: %s couldn't be opened\n", argv [2]);
        return (1);
    };
    while (fgets (buf, 259, fh) != NULL) {
        i = strlen (buf) - 1;
        while ((i >= 0) && ((buf [i] == '\n') || (buf [i] == ' '))) i--;
        buf [++i] = 0;

        a = strstr (buf, " ");
        if (a) {
            a [0] = 0;
            a++;

            s = (sym_ptr)malloc (sizeof (sym_t));
            s->sname = (char *)malloc (strlen (buf) + 1);
            s->name = (char *)malloc (strlen (a) + 1);

            strcpy (s->sname, buf);
            strcpy (s->name, a);

            s->next = shead;
            shead = s;
        };
    };
    fclose (fh);

#ifndef JCC
    esdrec [0] = __ascii_to_ebcdic ['E'];
    esdrec [1] = __ascii_to_ebcdic ['S'];
    esdrec [2] = __ascii_to_ebcdic ['D'];

    xsdrec [0] = __ascii_to_ebcdic ['X'];
    xsdrec [1] = __ascii_to_ebcdic ['S'];
    xsdrec [2] = __ascii_to_ebcdic ['D'];
#else
    esdrec [0] = 'E';
    esdrec [1] = 'S';
    esdrec [2] = 'D';

    xsdrec [0] = 'X';
    xsdrec [1] = 'S';
    xsdrec [2] = 'D';
#endif

    fho = fopen (argv [3], "wb");
    if (fho == NULL) {
        fprintf (stderr, "There was a problem opening the target object: %s.\n", argv [3]);
        return (1);
    };

    if (copy (argv [1], fho)) {
        fprintf (stderr, "There was a problem building the target object.\n");
        return (1);
    };

    fclose (fho);

    return (0);
};
