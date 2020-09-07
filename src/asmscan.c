/*
*    Copyright (c) 2003, Jason Paul Winter - All Rights Reserved     *
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct replace * repl_ptr;
typedef struct replace {
    char   * oldname;
    long     newname;
    long     replace;
    long     aliased;
    repl_ptr next;
};

repl_ptr head = NULL;
long st   = 0;
long line = 0;

static unsigned char __ebcdic_to_ascii[] = {
    "\x00\x01\x02\x03\x9C\x09\x86\x7F\x97\x8D\x8E\x0B\x0C\x0D\x0E\x0F\x10\x11\x12\x13\x9D\x0A\x08\x87\x18\x19\x92\x8F\x1C\x1D\x1E\x1F"
    "\x80\x81\x82\x83\x84\x85\x17\x1B\x88\x89\x8A\x8B\x8C\x05\x06\x07\x90\x91\x16\x93\x94\x95\x96\x04\x98\x99\x9A\x9B\x14\x15\x9E\x1A"
    "\x20\xA0\xE2\xE4\xE0\xE1\xE3\xE5\xE7\xF1\xA2\x2E\x3C\x28\x2B\x7C\x26\xE9\xEA\xEB\xE8\xED\xEE\xEF\xEC\xDF\x21\x24\x2A\x29\x3B\x5E"
    "\x2D\x2F\xC2\xC4\xC0\xC1\xC3\xC5\xC7\xD1\xA6\x2C\x25\x5F\x3E\x3F\xF8\xC9\xCA\xCB\xC8\xCD\xCE\xCF\xCC\x60\x3A\x23\x40\x27\x3D\x22"
    "\xD8\x61\x62\x63\x64\x65\x66\x67\x68\x69\xAB\xBB\xF0\xFD\xFE\xB1\xB0\x6A\x6B\x6C\x6D\x6E\x6F\x70\x71\x72\xAA\xBA\xE6\xB8\xC6\xA4"
    "\xB5\x7E\x73\x74\x75\x76\x77\x78\x79\x7A\xA1\xBF\xD0\x5B\xDE\xAE\xAC\xA3\xA5\xB7\xA9\xA7\xB6\xBC\xBD\xBE\xDD\xA8\xAF\x5D\xB4\xD7"
    "\x7B\x41\x42\x43\x44\x45\x46\x47\x48\x49\xAD\xF4\xF6\xF2\xF3\xF5\x7D\x4A\x4B\x4C\x4D\x4E\x4F\x50\x51\x52\xB9\xFB\xFC\xF9\xFA\xFF"
    "\x5C\xF7\x53\x54\x55\x56\x57\x58\x59\x5A\xB2\xD4\xD6\xD2\xD3\xD5\x30\x31\x32\x33\x34\x35\x36\x37\x38\x39\xB3\xDB\xDC\xD9\xDA\x9F"
};

char * specials [] = {"@REGION_", "R", /* Hard Coded to replace one extra _ with @ */
                      NULL}; /* pairs, second must be smaller - NULL terminated list */

#ifndef WIN32
#ifndef JCC
char * strupr (char * s) {
    long i = 0;

    while (s [i] = _toupper (s [i])) i++;

    return (s);
};
#endif
#endif

long alterlbl (char * l) {
    long i;
    long j;
    char c;

    i = strlen (l);
    if (i > 8) return (1);
    j = 0;
    while (j < i) {

#ifdef JCC
        c = __ebcdic_to_ascii [l [j]];
        if (((c >= __ebcdic_to_ascii ['a']) &&
             (c <= __ebcdic_to_ascii ['z'])) ||
            (l [j] == '_')) return (1);
#else
        c = l [j];
        if (((c >= 'a') &&
             (c <= 'z')) ||
            (c == '_')) return (1);
#endif

        j++;
    };

    return (0);
};

long add_repl (FILE * f, char * name) {
    repl_ptr t;

    t = head;
    while (t) {
        if (strcmp (name, t->oldname) == 0) break;
        t = t->next;
    };
    if (t == NULL) { /* Add the missing label */
        t = (repl_ptr)malloc (sizeof (struct replace));
        
        t->replace = alterlbl (name);
        t->aliased = 0;

        t->next = head;
        t->oldname = (char *)malloc (strlen (name) + 1);
        strcpy (t->oldname, name);
        t->newname = st++;
        head = t;

        if (f == NULL) {
            printf ("Second pass found new label: %s\n", name);
            exit (8);
        } else {
            fprintf (f, "ST%05d %s\n", t->newname, name);
        };
    };

    return (t->newname);
};

void getline_asm (FILE * f, char * b, FILE * comments) {
    long   i;
    long   cont = 0;
    char * a;

    a = fgets (b, 82, f);
    line++;
    while (a) {
        i = strlen (b);
        while ((i >= 0) && ((b [i] == 0) || (b [i] == ' ') || (b [i] == '\n') || (b [i] == '\r'))) i--;
        if ((b [0] != '*') && (cont == 0)) {
            ++i;
        } else {

            if (comments) {
                b [i+1] = 0;
                fprintf (comments, "%s\n", b);
            };

            if (i >= 71) {
                cont = 1; /* continuation char found on a comment */
            } else {
                cont = 0;
            };
            i = 0;
        };
        if (i) {
            b [i] = 0;
            break;
        } else {
            a = fgets (b, 82, f);
            line++;
        };
    };

    if (a == NULL) {
        b [0] = 0;
    };
};

void getcont (FILE * f, char * b, long * cont) {
    long   i;
    char * a;

    *cont = 0;

    a = fgets (b, 82, f);
    line++;
    if (a) {
        i = strlen (b);
        while ((i >= 0) && ((b [i] == 0) || (b [i] == ' ') || (b [i] == '\n') || (b [i] == '\r'))) i--;
        if (i >= 71) {
            *cont = 1;
            i--;
            while ((i >= 0) && ((b [i] == 0) || (b [i] == ' ') || (b [i] == '\n') || (b [i] == '\r'))) i--;
        };
        b [++i] = 0;
    } else {
        b [0] = 0;
    };
};

long check_op (char * o) {

    if (strcmp (o, "CSECT") == 0) return 1;
    if (strcmp (o, "RSECT") == 0) return 1;
    if (strcmp (o, "DSECT") == 0) return 1;
    if (strcmp (o, "END") == 0) return 1;

    return (0);
};

void stripline (char * buf, char * label, char * operation, char * parms, long * cont) {
    long   i;
    long   q = 0;
    char * a;
    char * b;

    label [0] = 0;
    operation [0] = 0;
    parms [0] = 0;
    *cont = 0;

    a = buf;
    if (buf [0] != ' ') {
        i = 0;
        while ((*a != ' ') && (*a != 0) && (i < 71)) label [i++] = *a++;
        label [i] = 0;
        if (i == 71) {
            printf ("Line %d Label is too long: %s\n", line, label);
            exit (8);
        };
    };
    while (*a == ' ') a++;
    if (*a == 0) {
        printf ("Line %d has bad format: %s\n", line, buf);
        exit (8);
    };

    i = 0;
    while ((*a != ' ') && (*a != 0) && (i < 9)) operation [i++] = *a++;
    operation [i] = 0;
    if (i == 9) {
        printf ("Line %d has bad operation: %s\n", line, operation);
        exit (8);
    };

    strupr (operation);
    if (check_op (operation)) {
        b = a;                     /* Probably shouldn't continue anyway! */
        while (*b) b++;
        if (b - buf > 71) *cont = 1;
        a [0] = 0;
    } else {
        b = a;
        i = 0;
        while (*a == ' ') {
            a++;
            i++;
        };
        if (i > 9) {               /* too many spaces, must be a comment following */
            a = b;
            while (*a) a++;
            if (b - buf > 71) *cont = 1;
            b [0] = 0;
        } else {
            if (*a != 0) {
                i = 0;
                while (((q) || (*a != ' ')) && (*a != 0)) {
                    if (*a == '\'') {
                        if (q) {
                            if (a [1] != '\'') {
                                q = 0;
                            } else {
                                parms [i++] = *a++;
                            };
                        } else {
                            q = 1;
                        };
                    };
                    parms [i++] = *a++;
                };
                parms [i] = 0;
                b = a;
                while (*b) b++;
                if (b - buf > 71) {
                    b--;
                    b [0] = 0; /* remove the continuation char */
                    *cont = 1;
                };
                a [0] = 0;
            } else {
                if (a - buf > 71) {
                    a--;
                    a [0] = 0; /* remove the continuation char */
                    *cont = 1;
                };
            };
        };
    };
};

void add_repl_str (FILE * fn, char * parms, char * l) { /* ALIAS has labels within single-quotes */
    long i;
    long j;
    long k;
    char name [80];
    char hex_find [] = "0123456789ABCDEF";
    repl_ptr t;

    if ((parms [0] == 'C') || (parms [0] == 'c')) {
        k = 2;
        i = 0;
        while (parms [k] != '\'') {
            name [i++] = parms [k++];
        };
    } else
    if ((parms [0] == 'X') || (parms [0] == 'x')) {
        strupr (parms);
        k = 2;
        i = 0;
        while (parms [k] != '\'') {
            j  = (strchr (hex_find, parms [k++]) - hex_find) * 16;
            j +=  strchr (hex_find, parms [k++]) - hex_find;
#ifndef JCC
            name [i++] = __ebcdic_to_ascii [j];
#else
            name [i++] = j;
#endif
        };
    };
    name [i] = 0;

    i = add_repl (fn, name);

    t = head;
    while (t) {
        if (strcmp (l, t->oldname) == 0) break;
        t = t->next;
    };
    if (t != NULL) { /* Add the missing label */
        if (t->newname != i) {
            t->newname = i;
            t->aliased = 1;
            t->replace = alterlbl (name);
        };
    } else {
        printf ("ALIAS: %s doesn't link to an owner label %s\n", name, l);
        exit (8);
    };

};

/* ALIAS has labels within single-quotes */
/* No longer need ALIAS to be written out
void fix_parms_q (char * parms) {
    long i;
    long j;
    long k;
    char name [80];
    char hex_find [] = "0123456789ABCDEF";

    if ((parms [0] == 'C') || (parms [0] == 'c')) {
        k = 2;
        i = 0;
        while (parms [k] != '\'') {
            name [i++] = parms [k++];
        };
    } else
    if ((parms [0] == 'X') || (parms [0] == 'x')) {
        strupr (parms);
        k = 2;
        i = 0;
        while (parms [k] != '\'') {
            j  = (strchr (hex_find, parms [k++]) - hex_find) * 16;
            j +=  strchr (hex_find, parms [k++]) - hex_find;
#ifndef JCC
            name [i++] = __ebcdic_to_ascii [j];
#else
            name [i++] = j;
#endif
        };
    };
    name [i] = 0;

    i = add_repl (NULL, name);

    sprintf (parms, "C\'ST%05d\'", i);
};
*/
void fix_special (char * parms) {
    char   * a;
    char   * b;
    long     i;
    long     r;

    i = 0;
    while (specials [i]) {
        a = strstr (parms, specials [i]);
        while (a) {
            r = strlen (specials [i + 1]);
            strncpy (a, specials [i + 1], r);
            b = a + strlen (specials [i]);
            a += r;
            strcpy (a, b);
            if (i == 0) { /* special case _'s to @'s */
                b = a;
                while (*b != '_') b++;
                b [0] = '@';
            };
            a = strstr (parms, specials [i]);
        };
        i += 2;
    };
};

void fix_parms (char * parms) {
    char   * a;
    char   * b;
    char   * c;
    char   * d;
    long     i;
    long     j;
    long     r;
    char     work [512];
    char     newl [9];
    repl_ptr t;
    repl_ptr z;
    char     check [] = "~`!%^&*()-+=|\\{}[]:;\"\'?/<>,. "; /* these break up labels */

    fix_special (parms);

    a = parms;
    work [0] = 0;

    c = strstr (a, "\'"); /* any quotes to worry about? */
    if (c) c [0] = 0;

    while (1) {
        t = head;
        z = NULL;
        d = NULL;
        while (1) {
            while (t) {
                if (((t->replace) || (t->aliased)) && (b = strstr (a, t->oldname))) {
                    i = strlen (t->oldname);
                    r = 1;
                    if (b != a) { /* start of string doesn't need break chars */
                        if (strchr (check, b [-1]) == NULL) r = 0;
                    };
                    if (b [i]) {        /* end of string doesn't need break chars */
                        if (strchr (check, b [i]) == NULL) r = 0;
                    };
                    if (r) break;
                };
                t = t->next;
            };

            if (t) {
                if ((d == NULL) || (d > b)) {
                    d = b;
                    j = i;
                    z = t;
                };
                t = t->next;
            } else {
                t = z;
                i = j;
                b = d;
                break;
            };
        };
        if (t) {
            b [0] = 0;
            strupr (a);          /* non-strings get case converted */
            strcat (work, a);

            if (t->replace) {
                sprintf (newl, "ST%05d", t->newname);
                strcat (work, newl);

            } else { /* must be an aliased label, which is acceptable as-is */

                j = t->newname;

                t = head;
                while (t) {
                    if ((j == t->newname) && (t->aliased == 0)) break;
                    t = t->next;
                };

                strcat (work, t->oldname);
            };
            
            a = b + i;
        } else {
            strupr (a);          /* non-strings get case converted */
            strcat (work, a);
            if (c) {
                c [0] = '\'';
                a = c + 1;
                while (1) {
                    if (*a == '\'') {
                        if (a [1] != '\'') {
                            break;
                        } else {
                            a++;
                        };
                    };
                    a++;
                    if (*a == 0) {
                        printf ("String found without an end quote: %s\n", parms);
                        exit (8);
                    };
                };
                a++;
                i = *a;
                a [0] = 0;
                strcat (work, c);
                a [0] = (char)i;
                c = strstr (a, "\'"); /* any more quotes to worry about? */
                if (c) c [0] = 0;
            } else {
                break;
            };
        };
    };

    strcpy (parms, work);
};

void fix_buf (char * buf, char * label, char * oper, char * parms) {
    char   newlabel [9];
    char * p;
    long   i;
    repl_ptr t;

    if (strcmp (oper, "ALIAS") == 0) {
/*        if (parms [0]) fix_parms_q (parms);*/ /* No longer need ALIAS to be written out */
        return; /* So return instead. */
    } else {
        if (parms [0]) {
            if (strcmp (oper, "DC") == 0) { /* Try to allow F,A,V,DL8,X etc... */

                if ((strncmp (parms, "X'", 2) == 0) ||
                    (strncmp (parms, "C'", 2) == 0) ||
                    (strncmp (parms, "E'", 2) == 0) ||
                    (strncmp (parms, "H'", 2) == 0) ||
                    (strncmp (parms, "A(", 2) == 0) ||
                    (strncmp (parms, "V(", 2) == 0) ||
                    (strncmp (parms, "F'", 2) == 0))

                    p = &(parms [1]);
                else
                if (strncmp (parms, "DL8'", 4) == 0)
                    p = &(parms [3]);
                else
                    p = parms;

            } else
                p = parms;

            fix_parms (p);
        };

        if (strcmp (oper, "DCCPRLG") == 0) {
            strcpy (oper, "DCCPR38");
        } else
        if (strcmp (oper, "DCCPRLL") == 0) { /* special library version */
            strcpy (oper, "DCCPL38");
        } else
        if (strcmp (oper, "JCCPRLG") == 0) {
            strcpy (oper, "JCCPR38");
        } else
        if (strcmp (oper, "JCCPRLL") == 0) { /* special library version */
            strcpy (oper, "JCCPL38");
        };
    };

    if (label [0]) {
        fix_special (label);

        t = head;
        while (t) {
            if (strcmp (label, t->oldname) == 0) break;
            t = t->next;
        };
        if (t == NULL) {
            printf ("Second pass found new label: %s\n", label);
            exit (8);
        };
        i = t->newname;

        if (t->aliased) {
            t = head;
            while (t) {
                if ((i == t->newname) && (t->aliased == 0)) break;
                t = t->next;
            };

            if (alterlbl (t->oldname)) {
                sprintf (newlabel, "ST%05d", i);
                sprintf (buf, "%s %-5s %s", newlabel, oper, parms);
            } else {
                sprintf (buf, "%-7s %-5s %s", t->oldname, oper, parms);
            };
        } else {
            if (alterlbl (label)) {
                sprintf (newlabel, "ST%05d", i);
                sprintf (buf, "%s %-5s %s", newlabel, oper, parms);
            } else {
                sprintf (buf, "%-7s %-5s %s", label, oper, parms);
            };
        };

    } else {
        sprintf (buf, "        %-5s %s", oper, parms);
    };
};

char * get_repl (char * name) {
    repl_ptr s;
    repl_ptr t;

    t = head;
    while (t) {
        if (strcmp (name, t->oldname) == 0) break;
        t = t->next;
    };
    if (t != NULL) { /* Find the alias label if one exists */
        s = head;
        while ((s) && (s->newname != t->newname)) {
            s = s->next;
        };
        if (s) t = s; /* It may find just the normal label */
        return (t->oldname);
    } else {
        return (NULL); /* Should never happen */
    }
};

void process_files (FILE * f, FILE * fo, FILE * fn) {
    long cont;
    char buf [83]; /* FB80 input, with newline & terminator + 1 */
    char label     [72]; /* max label length = 71 */
    char operation [10];  /* no operation code should be > 8 */
    char parms     [512];
    long skipwr = 0;

    while (1) {
        getline_asm (f, buf, NULL);
        if (buf [0]) {
            stripline (buf, label, operation, parms, &cont);

            if (label [0]) {
                fix_special (label);
                add_repl (fn, label);
            };

            if (strcmp (operation, "ALIAS") == 0) {
                add_repl_str (fn, parms, label);
            };

            while (cont) {
                getcont (f, buf, &cont);
            };
        } else {
            break;
        };
    };

    fseek (f, 0, SEEK_SET);

    line = 0;

    strcpy (buf, "* Pre-Processed by ASMSCAN, written by Jason Winter.");
    while (1) {
        if (skipwr == 0) {
            if (strlen (buf) > 71) {
                printf ("A replaced label made the line too long:\n%s\n", buf);
                exit (8);
            };
            fprintf (fo, "%s\n", buf);
        };
        skipwr = 0;
        getline_asm (f, buf, fo);
        if (buf [0]) {
            stripline (buf, label, operation, parms, &cont);

            fix_buf (buf, label, operation, parms);

            if ((strcmp (operation, "AMODE") == 0) ||
                (strcmp (operation, "RMODE") == 0) ||
                (strcmp (operation, "ALIAS") == 0)) { /* Don't write out ALIAS statements now */
                skipwr = 1;
            };

            if (strcmp (operation, "CSECT") == 0) {
                fprintf (fo, "* OLD_CSECT: %s\n*\n", get_repl (label));
            };

            while (cont) {
                if (skipwr == 0) {
                    if (strlen (buf) > 71) {
                        printf ("A replaced label made the line too long:\n%s\n", buf);
                        exit (8);
                    };
                    fprintf (fo, "%-71s+\n", buf);
                };
                getcont (f, buf, &cont);
                fix_parms (buf);
            };
        } else {
            break;
        };
    };
};

int main (int argc, char * argv []) {
    FILE * f;
    FILE * fo;
    FILE * fn;
    int    rc = 0;

    if (argc != 4) {
        printf ("Usage: asmscan input.asm output.asm output.nam\n");
        printf (" Converts longnames to shortnames and creates a translation file.\n");
        return (8);
    };

    f = fopen (argv [1], "r");
    if (f) {
        fo = fopen (argv [2], "w");
        if (fo) {
            fn = fopen (argv [3], "w");
            if (fn) {
                process_files (f, fo, fn);
                fclose (fn);
            } else {
                printf ("File <%s> could not be opened.\n", argv [3]);
                rc = 8;
            };
            fclose (fo);
        } else {
            printf ("File <%s> could not be opened.\n", argv [2]);
            rc = 8;
        };
        fclose (f);
    } else {
        printf ("File <%s> could not be opened.\n", argv [1]);
        rc = 8;
    };

    return (rc);
};
