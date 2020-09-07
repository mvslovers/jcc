/*
*    Copyright (c) 2003, Jason Paul Winter - All Rights Reserved     *
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#ifndef WIN32
#ifndef JCC
#define stricmp strcasecmp
#endif
#endif

static char replace_prefix [3] = "ST"; /* Allow user to override replacement string - but only 2 chars */

static long   errors = 0;

static FILE * testout = NULL;
static long   testadx = 0;

long shortnames = 0;  /* ST000000-ST999999 */
long forced     = 0;  /* Are we forcing Weak Externals to be linked? */
long rename_sym = 0;  /* Are we renaming all symbols (1), only long symbols (0) or no symbols at all (-1) */

typedef struct sym_tag * sym_ptr;
typedef struct lnk_tag * lnk_ptr;
typedef struct fil_tag * fil_ptr;
typedef struct lfl_tag * lfl_ptr;

typedef struct sym_tag {
    char  * name;      /* Long Name */
    long    shortname; /* STnnnnn or -1 */
    long    reported;  /* If an error has already been displayed/weak external */
    lfl_ptr uses;      /* List of files which use the weak external */
    fil_ptr file;      /* Where defined (first only counts) */
    sym_ptr next;
} sym_t;

typedef struct lfl_tag {
    fil_ptr file;      /* List of files required */
    lfl_ptr next;      /* List of links */
} lfl_t;

typedef struct lnk_tag {
    sym_ptr symbols;   /* List of symbols required */
    lnk_ptr next;      /* List of links */
} lnk_t;

typedef struct fil_tag {
    char  * file;      /* File name */
    long    include;   /* If file needs to be included */
    lnk_ptr head;      /* List of links */
    fil_ptr next;      /* List of files */
} fil_t;

fil_ptr fhead = NULL;
fil_ptr ftail = NULL;
sym_ptr shead = NULL;

char esdrec [3];
char xsdrec [3];

char fncrec [3]; /* Function MVC instruction for RENT dereferences */
char txtrec [3]; /*  ... where the MVCs are */
char prdrec [3]; /* PreLink relocation for obj in rent data reference */
char rrdrec [3]; /* Runtime relocation for rent data to rent data ref */
char rntrec [3]; /* Initialisation text (RNT->TXT) */
char rndrec [3]; /* Relocations for External References (RND->RLD) */

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
    }
}

void etoa (char * d, long len) {
    long i;

    i = 0;
    while (i < len) {
        d [i] = __ebcdic_to_ascii [((unsigned char *)d) [i]];
        i++;
    }
}

#else

#define byteswap(i) (i)
#define wordswap(i) (i)

void atoe (char * d, long len) {
}

void etoa (char * d, long len) {
}

#endif

/* RENT PRELINK ADDITIONS */

typedef struct adx_tag * adx_ptr;
typedef struct adx_tag * rrels_ptr;
typedef struct adx_tag * funcs_ptr;
typedef struct adx_tag {
    long    offset;
    long    noffset;
    adx_ptr next;
} adx_t;

static funcs_ptr fchead = NULL;
static funcs_ptr fctail = NULL;

static rrels_ptr rhead = NULL;
static rrels_ptr rtail = NULL;

void new_adx (long offset, funcs_ptr * fch, funcs_ptr * fct, long new_offset) {

    if (*fch == NULL) {
        *fch = malloc (sizeof (adx_t));
        (*fch)->next = NULL;
        (*fch)->offset = offset;
        (*fch)->noffset = new_offset;
        *fct = *fch;
    } else {
        (*fct)->next = malloc (sizeof (adx_t));
        *fct = (*fct)->next;
        (*fct)->offset = offset;
        (*fct)->noffset = new_offset;
        (*fct)->next = NULL;
    }
}

typedef struct adxt_tag * prels_ptr;
typedef struct adxt_tag * adxt_ptr;
typedef struct adxt_tag {
    long     offset;
    long     object;
    adxt_ptr next;
} adxt_t;

static prels_ptr phead = NULL;
static prels_ptr ptail = NULL;

void new_adxt (long offset, long object, adxt_ptr * fch, adxt_ptr * fct) {

    if (*fch == NULL) {
        *fch = malloc (sizeof (adx_t));
        (*fch)->next = NULL;
        (*fch)->offset = offset;
        (*fch)->object = object;
        *fct = *fch;
    } else {
        (*fct)->next = malloc (sizeof (adx_t));
        *fct = (*fct)->next;
        (*fct)->offset = offset;
        (*fct)->object = object;
        (*fct)->next = NULL;
    }
}

static long object_sequence = 0;
static long rent_module = 0;

typedef struct rec_tag * rec_ptr;
typedef struct rec_tag {
    char    line [80];
    rec_ptr next;
} rec_t;

static rec_ptr rec1head = NULL;
static rec_ptr rec1tail = NULL;

void new_rec (char * line, rec_ptr * rch, rec_ptr * rct) {

    if (*rch == NULL) {
        *rch = malloc (sizeof (rec_t));
        (*rch)->next = NULL;
        memcpy ((*rch)->line, line, 80);
        *rct = *rch;
    } else {
        (*rct)->next = malloc (sizeof (rec_t));
        *rct = (*rct)->next;
        memcpy ((*rct)->line, line, 80);
        (*rct)->next = NULL;
    }
}

#define SD 0
#define LD 1
#define ER 2

static long rent_ids = 1;

typedef struct xsd_tag * xsd_ptr;
typedef struct xsd_tag {
    char  * name;
    long    type;
    long    id;
    long    offset;
    xsd_ptr next;
} xsd_t;

static xsd_ptr xsdhead = NULL;
static xsd_ptr xsdtail = NULL;

static long new_xsd (char * buf, long len, long * type, long old_offset) {
    xsd_ptr xp;
    char    name [41];
    long    t;

    t = *type;
    t = wordswap (t);

    memcpy (name, buf, len);
    etoa (name, len);
    name [len] = 0;

    xp = xsdhead;
    while (xp) {

        if (strcmp (xp->name, name) == 0) {
            
            if ((t >> 24) == LD)
                if (xp->offset != -1) {
                    fprintf (stderr, "Unexpected duplicate symbol <%s, %08X != %08X> in RENT data.\n", name, xp->offset, (t & 0x00FFFFFF) + old_offset);
                    errors++;
                    exit (8);
                } else
                    xp->offset = (t & 0x00FFFFFF) + old_offset;

            xp->type |= (t >> 24);

            return (xp->id);
        }
        xp = xp->next;
    }

    if (xsdhead == NULL) {
        xsdhead = (xsd_ptr)malloc (sizeof (xsd_t));
        xsdhead->name = (char *)malloc (len + 1);
        strcpy (xsdhead->name, name);
        xsdhead->type = (t >> 24);
        if (xsdhead->type == LD) {
            xsdhead->offset = (t & 0x00FFFFFF) + old_offset;
        } else {
            xsdhead->offset = -1;
        }
        xsdhead->id = ++rent_ids;
        xsdhead->next = NULL;
        xsdtail = xsdhead;

//        printf ("XSD added: %s, %d\n", xsdhead->name, xsdhead->offset);
    } else {
        xsdtail->next = (xsd_ptr)malloc (sizeof (xsd_t));
        xsdtail = xsdtail->next;

        xsdtail->name = (char *)malloc (len + 1);
        strcpy (xsdtail->name, name);
        xsdtail->type = t >> 24;
        if (xsdtail->type == LD) {
            xsdtail->offset = (t & 0x00FFFFFF) + old_offset;
        } else {
            xsdtail->offset = -1;
        }
        xsdtail->id = ++rent_ids;
        xsdtail->next = NULL;

//        printf ("XSD added: %s, %d\n", xsdtail->name, xsdtail->offset);
    }

    return (rent_ids);
}

static long bend = 0;

typedef struct rlc_tag * rlc_ptr;
typedef struct rlc_tag {
    long    offset;
    long    v;
    xsd_ptr xp;
    rlc_ptr next;
} rlc_t;

static rlc_ptr rlchead = NULL;
static rlc_ptr rlctail = NULL;

void new_reloc (long offset, long id, long v) {
    xsd_ptr xp;

    xp = xsdhead;
    while (xp) {
        if (xp->id == id) break;
        xp = xp->next;
    }
/*
    if (xp == NULL) {
        printf ("id:%d not found for offset:%08X (v=%d)\n", id, offset, v);
    } else {
        printf ("Linked %s with id:%d\n", xp->name, id);
    }
*/
    if (rlchead == NULL) {
        rlchead = malloc (sizeof (rlc_t));
        rlchead->next = NULL;
        rlchead->offset = offset;
        rlchead->xp = xp;
        rlchead->v = v;
        rlctail = rlchead;
    } else {
        rlctail->next = malloc (sizeof (rlc_t));
        rlctail = rlctail->next;
        rlctail->offset = offset;
        rlctail->xp = xp;
        rlctail->v = v;
        rlctail->next = NULL;
    }
}

long o_cnt = 1;
long o_offsets [1024];

static char plus;

/* END RENT ADDITIONS */

int copy (char * file, FILE * fho) {
    unsigned char buf1 [81];
    unsigned char buf2 [81];
    char   name [41];
    FILE * fh;
    long   i;
    long   j;
    long   k;
    long   l;
    long   m;
    sym_ptr sym;
    long x_trigger = 0;
    long dowrite = 1;
    long id = rent_ids;
    long old_offset = bend;
    long old_testadx = testadx;
    short a_ids [1024];

    printf ("Linking <%s>\n", file);

    fh = fopen (file, "rb");
    if (fh != NULL) {
        while (fread (buf1, 1, 80, fh) == 80) {

            if (buf1 [0] == 2) { /* Object Record */

                if (memcmp (&(buf1 [1]), fncrec, 3) == 0) {
                    dowrite = 0;

                    i = *((long *)(&buf1 [4]));
                    i = wordswap (i);
                    k = 1;
                    while (k <= i) {
                        j = *((long *)(&buf1 [4 + (4 * k++)]));
                        j = wordswap (j);
                        new_adx (j, &fchead, &fctail, 0);

//                        printf ("offset %08X to be updated (of %d)\n", j, i);
                    }

                } else if (memcmp (&(buf1 [1]), txtrec, 3) == 0) {
                    if (x_trigger == 0) {
                        x_trigger = 1;

                        if (fchead) {
                            object_sequence += 4;

                            if (o_cnt == 1024) {
                                fprintf (stderr, "Out of RENT block pointers (1023 limit.)\n");
                                errors++;
                                exit (8);
                            }

                            o_offsets [o_cnt++] = old_offset;
                        }
                    }

                    while (fchead) {

                        i = (*((long *)(&buf1 [4])));
                        i = wordswap (i) & 0x00FFFFFF;
                        j = *((short *)(&buf1 [10]));
                        j = i + byteswap (j);
                        k = fchead->offset + 4; /* MVC's 2 byte move-from offset */

//                        printf ("f_address %08X to %08X checked\n", i, j);

                        if ((k >= i) && (k < j)) {

//                            printf ("f_address %08X updated\n", k);

                            i = 16 + (k - i);
                            buf1 [i] |= (object_sequence >> 8) & 0x0F;
                            /* Does the record continue? */
                            if (k + 1 < j) {
                                buf1 [i + 1] = object_sequence & 0xFF;
                            } else {
                                fwrite (buf1, 1, 80, fho);
                                if (fread (buf1, 1, 80, fh) == 80) {
                                    buf1 [16] = object_sequence & 0xFF;
                                } else {
                                    dowrite = 0;
                                    break;
                                }
                            }

                            /* Remove Entry */
                            fctail = fchead;
                            fchead = fchead->next;
                            free (fctail);
                        } else {
                            break;
                        }
                    }

                } else if (memcmp (&(buf1 [1]), prdrec, 3) == 0) {
                    dowrite = 0;

                    i = *((long *)(&buf1 [4]));
                    i = wordswap (i);
                    k = 1;
                    while (k <= i) {
                        j = *((long *)(&buf1 [4 + (4 * k++)]));
                        j = wordswap (j);
                        new_adxt (j + old_offset, old_testadx, &phead, &ptail);

//                        printf ("offset %08X to be updated prelink\n", j);
                    }

                } else if (memcmp (&(buf1 [1]), rrdrec, 3) == 0) {
                    dowrite = 0;

                    i = *((long *)(&buf1 [4]));
                    i = wordswap (i);
                    k = 1;
                    while (k <= i) {
                        j = *((long *)(&buf1 [4 + (4 * k++)]));
                        j = wordswap (j);
                        new_adx (j + old_offset, &rhead, &rtail, old_offset);

//                        printf ("offset %08X to be updated runtime\n", j);
                    }

                } else if (memcmp (&(buf1 [1]), rntrec, 3) == 0) {
                    dowrite = 0;

                    i = (*((long *)(&buf1 [4])));
                    i = wordswap (i);
                    j = *((short *)(&buf1 [10]));
                    j = i + byteswap (j);

                    i += old_offset;
                    i = wordswap (i);
                    *((long *)(&buf1 [4])) = i;
                    j += old_offset;

                    if (j > bend) {
                        bend = j;
                        bend = ((bend + 7) & 0xFFFFFFF8);
                    }

                    memcpy (&(buf1 [1]), txtrec, 3);

                    new_rec (buf1, &rec1head, &rec1tail);

                } else if (memcmp (&(buf1 [1]), rndrec, 3) == 0) {
                    dowrite = 0;

                    k = 16;
                    i = *((short *)&(buf1 [10]));
                    i = byteswap (i) + 16;
                    l = 1;
                    while (k < i) {
                        if (l) {
                            j = *((short *)&(buf1 [k]));
                            j = byteswap (j);
                            k += 4;
                        }

                        if ((buf1 [k] & 0x80) == 0)
                            l = 1;
                        else
                            l = 0;

                        m = *((long *)&(buf1 [k]));
                        m = wordswap (m) & 0x3FFFFFFF;
                        k += 4;

//                        printf ("Adding reloc: %d, %d\n", m + old_offset, j);

                        new_reloc (m + old_offset, a_ids [j], (buf1 [k] & 0x40));
                    }

                } else if ((buf1 [72] == plus) && (memcmp (&(buf1 [1]), xsdrec, 3) == 0)) { /* Can only be END or XSD */
                    dowrite = 0;

                    if (buf1 [24] == SD) {

                        // Just has the length... Ignore it.

                    } else {
                        i = new_xsd (&(buf1 [32]), buf1 [19], (long *)&(buf1 [24]), old_offset);

                        if (buf1 [24] == ER) {
                            j = *((short *)&(buf1 [14]));
                            j = byteswap (j);

//                            printf ("a_ids [%d] = %d\n", j, i);

                            a_ids [j] = (short)i;

                        }
/*
                        buf1 [72] = 0;
                        etoa (&(buf1 [32]), 40);
                        printf ("Name added to RENT pool:%s\n", &(buf1 [32]));
*/
                    }

                } else if ((rename_sym == 1) && (strncmp (esdrec, &(buf1 [1]), 3) == 0)) { /* ESD */

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
                            if (strcmp (sym->name, name) == 0) break;
                            sym = sym->next;
                        }
                        if (sym == NULL) {
                            fprintf (stderr, "Name <%s> not found.\n", name);
                            errors++;
                            exit (8);
                        }
                        if (sym->shortname != -1) {     /* No reason why it isn't! */
                            sprintf (name, "%s%06d", replace_prefix, sym->shortname);
                            atoe (name, 8);
                            memcpy (&(buf1 [j]), name, 8);
                        }

                        if (buf1 [8  + j] == 0x00) { /* Type=SD, has flags which we may modify here */
                            buf1 [12 + j] =  0x07;   /* rmode=any, amode=any */

                            if (testout != NULL)
                                fprintf (testout, "%08X %s\n", testadx, sym->name);
                            testadx += (wordswap(*((long *)&(buf1 [12 + j]))) & 0xFFFFFF) + 7;
                            testadx &= 0xFFFFFFF8;
                        }

                        j += 16;
                        i -= 16;
                    }

                } else if ((rename_sym != -1) && (strncmp (xsdrec, &(buf1 [1]), 3) == 0)) { /* XSD */

                    memcpy (buf2, buf1, 80);
                    etoa (buf2, 80);

                    i = wordswap (*((long *)(&buf1 [16]))); /* Name Length */
                    strncpy (name, &(buf2 [32]), i); /* Name */
                    name [i] = 0;

                    sym = shead;
                    while (sym) {
                        if (strcmp (sym->name, name) == 0) break;
                        sym = sym->next;
                    }
                    if (sym == NULL) {
                        fprintf (stderr, "Name <%s> not found.\n", name);
                        errors++;
                        exit (8);
                    }

                    memcpy (buf2, buf1, 80);

                    memset (&(buf1 [4]), 0x40, 68);    /* Blank the record */
                    buf1 [1] = esdrec [0];             /* Convert to ESD */
                    buf1 [10] = 0;
                    buf1 [11] = 16;
                    memcpy (&(buf1 [14]), &(buf2 [14]), 2); /* ESDID */
                    if (sym->shortname != -1) {
                        sprintf (&(buf1 [16]), "%s%06d", replace_prefix, sym->shortname);
                    } else {
                        sprintf (&(buf1 [16]), "%-8s", sym->name);
                    }
                    atoe (&(buf1 [16]), 8);
                    memcpy (&(buf1 [24]), &(buf2 [24]), 8); /* ESDType, Addr, Flag, Len/LDID */

                    if (buf1 [24] == 0x00) { /* Type=SD, has flags which we may modify here */
                        buf1 [28] =  0x07;   /* rmode=any, amode=any */

                        if (testout != NULL)
                            fprintf (testout, "%08X %s\n", testadx, sym->name);
                        testadx += (wordswap(*((long *)&(buf1 [28]))) & 0xFFFFFF) + 7;
                        testadx &= 0xFFFFFFF8;
                    }

                } else if (strncmp (esdrec, &(buf1 [1]), 3) == 0) { /* ESD - no renames */
                    
                    i = byteswap (*((short *)(&buf1 [10])));
                    j = 16;
                    while (i > 0) { /* variable number of bytes in record */

                        if (buf1 [8 + j] == 0x00) { /* Type=SD */

                            memcpy (name, &(buf1 [j]), 8); /* Get Name */
                            etoa (name, 8);
                            k = 8;
                            while ((k) && (name [k-1] == ' ')) k--;
                            name [k] = 0;

                            if (testout != NULL)
                                fprintf (testout, "%08X %s\n", testadx, name);
                            testadx += (wordswap(*((long *)&(buf1 [12 + j]))) & 0xFFFFFF) + 7;
                            testadx &= 0xFFFFFFF8;
                        }

                        j += 16;
                        i -= 16;
                    }

                } else if (strncmp (xsdrec, &(buf1 [1]), 3) == 0) { /* XSD - no renames */

                    if (buf1 [24] == 0x00) { /* Type=SD */

                        memcpy (buf2, buf1, 80);
                        etoa (buf2, 80);

                        i = wordswap (*((long *)(&buf1 [16]))); /* Name Length */
                        strncpy (name, &(buf2 [32]), i); /* Name */
                        name [i] = 0;

                        if (testout != NULL)
                            fprintf (testout, "%08X %s\n", testadx, name);
                        testadx += (wordswap(*((long *)&(buf1 [28]))) & 0xFFFFFF) + 7;
                        testadx &= 0xFFFFFFF8;
                    }
                }
            }

            if (dowrite)
                fwrite (buf1, 1, 80, fho);
            dowrite = 1;
        }
        fclose (fh);
        return (0);
    }

    fprintf (stderr, "File <%s> not found.\n", file);

    return (1);
}

static long sequence = 1;
static char sbuf [9];

static void rsbuf () {

    sprintf (sbuf, "%08d", sequence++);
    atoe (sbuf, 8);
}

static void fix_xsd_esd (char * buf1) {
    sym_ptr sym;
    long    i;
    char    name [41];
    char    buf2 [80];

    i = wordswap (*((long *)(&buf1 [16]))); /* Name Length */
    memcpy (name, &(buf1 [32]), i);
    etoa (name, i);
    name [i] = 0;

    sym = shead;
    while (sym) {
        if (strcmp (sym->name, name) == 0) break;
        sym = sym->next;
    }

    memcpy (buf2, buf1, 80);

    memset (&(buf1 [4]), 0x40, 68);    /* Blank the record */
    buf1 [1] = esdrec [0];             /* Convert to ESD */
    buf1 [10] = 0;
    buf1 [11] = 16;
    memcpy (&(buf1 [14]), &(buf2 [14]), 2); /* ESDID */
    if (sym == NULL) {
        name [8] = 0;
        sprintf (&(buf1 [16]), "%-8s", name);
    } else if (sym->shortname != -1) {
        sprintf (&(buf1 [16]), "%s%06d", replace_prefix, sym->shortname);
    } else {
        sprintf (&(buf1 [16]), "%-8s", sym->name);
    }
    atoe (&(buf1 [16]), 8);
    memcpy (&(buf1 [24]), &(buf2 [24]), 8); /* ESDType, Addr, Flag, Len/LDID */
}

int trav_output (FILE * fho) {
    fil_ptr fp = fhead;
    long    len;
    long    i;
    long    j;
    long    k;
    long    l;
    long    r;
    long    z;
    long    new_id;
    char    dl [80];
    char *  sl;
    char *  tl;
    rlc_ptr rp;
    rlc_ptr sp;
    prels_ptr pp;
    rrels_ptr rrp;
    long    table_offset;
    long    m8blks;
    long    mloop;
    long    ocnt = 0;
    xsd_ptr xp;
    rec_ptr rpr;
    rec_ptr rprt;

    printf ("\n");

    while (fp) {
        if (fp->include)
            if (copy (fp->file, fho))
                return (1);
        fp = fp->next;
    }

    if (rec1head) { /* Anything to process RENT-mode? */

        m8blks = bend >> 23; /* instead of 16MB limit, use a count + 8MB blocks */
        if ((bend) && ((bend & 0x007FFFFF) == 0)) m8blks--;
        mloop = m8blks;
        
        printf ("RENT Data Size: %d bytes (0x%08X)\n", bend, bend);

        do {
            dl [0] = 2;
            memcpy (&(dl [1]), xsdrec, 3);
            memset (&(dl [4]), 64, 6);

            len = 8;

            memset (&(dl [10]), 0, 22);

            dl [11] = (unsigned char)(0x10 + len);
            dl [15] = (unsigned char)1;
            dl [19] = (unsigned char)len;
            dl [23] = 1;

            dl [24] = (unsigned char)SD;

            dl [25] = 0;
            dl [26] = 0;
            dl [27] = 0;

            if ((m8blks != ocnt) || ((bend) && ((bend & 0x007FFFFF) == 0))) {
                dl [28] = 0x07;
                dl [29] = (char)0x80;
                dl [30] = 0x00;
                dl [31] = 0x00;
            } else {
                dl [28] = 0x07;
                dl [29] = (bend >> 16) & 0x7F;
                dl [30] = (bend >> 8) & 0xFF;
                dl [31] = bend & 0xFF;
            }

            memset (&(dl [32 + len]), 64, 40 - len);
            memcpy (&(dl [32]), "@@JCCRNT", len);
            if (ocnt) {
                if (((ocnt >> 4) & 0x0F) > 9)
                    dl [38] = 'A' + ((ocnt >> 4) & 0x0F) - 10;
                else
                    dl [38] = '0' + ((ocnt >> 4) & 0x0F);
                if ((ocnt & 0x0F) > 9)
                    dl [39] = 'A' + (ocnt & 0x0F) - 10;
                else
                    dl [39] = '0' + (ocnt & 0x0F);
            }
            atoe (&(dl [32]), len);

            rsbuf ();
            memcpy (&(dl [72]), sbuf, 8);

            if (rename_sym != -1)
                fix_xsd_esd (dl);

            fwrite (dl, 1, 80, fho);

            memset (&(dl [4]), 64, 6);

            len = 5;

            memset (&(dl [10]), 0, 22);

            dl [11] = (unsigned char)(0x10 + len);
            dl [15] = (unsigned char)2;
            dl [19] = (unsigned char)len;
            dl [23] = 1;

            dl [24] = (unsigned char)ER;

            dl [25] = 0;
            dl [26] = 0;
            dl [27] = 0;

            memset (&(dl [28]), 64, 4);

            memset (&(dl [32 + len]), 64, 40 - len);
            memcpy (&(dl [32]), "@crt0", len);
            atoe (&(dl [32]), len);

            rsbuf ();
            memcpy (&(dl [72]), sbuf, 8);

            if (rename_sym != -1)
                fix_xsd_esd (dl);

            fwrite (dl, 1, 80, fho);

            new_id = 2;
            xp = xsdhead;
            while (xp) {
                if (xp->type == ER) {
                    xp->id = ++new_id;

                    len = strlen (xp->name);

                    memset (&(dl [10]), 0, 22);

                    dl [11] = (unsigned char)(0x10 + len);
                    dl [14] = (unsigned char)(xp->id >> 8);
                    dl [15] = (unsigned char)xp->id;
                    dl [19] = (unsigned char)len;
                    dl [23] = 1;

                    dl [24] = (unsigned char)xp->type;

                    r = xp->offset;
                    if (r != -1) {
                        dl [25] = (r >> 16) & 0xFF;
                        dl [26] = (r >> 8) & 0xFF;
                        dl [27] = r & 0xFF;
                    }

                    memset (&(dl [28]), 64, 4);

                    memset (&(dl [32 + len]), 64, 40 - len);
                    memcpy (&(dl [32]), xp->name, len);
                    atoe (&(dl [32]), len);

                    rsbuf ();
                    memcpy (&(dl [72]), sbuf, 8);

                    if (rename_sym != -1)
                        fix_xsd_esd (dl);

                    fwrite (dl, 1, 80, fho);
                }

                xp = xp->next;
            }

            rpr = rec1head;
            while (rpr) {

                sl = rpr->line;

                i = (*((long *)(&sl [4])));
                i = wordswap (i);
                j = *((short *)(&sl [10]));
                j = i + byteswap (j);

                if ((i >> 23) == ocnt) {
                    if (((j - 1) >> 23) != ocnt) { /* ends into a new 8MB region */
                        rprt = rpr->next;
                        rpr->next = (rec_ptr)malloc (sizeof (rec_t));
                        rpr->next->next = rprt;
                        memcpy (rpr->next->line, rpr->line, 16);
                        memcpy (&(rpr->next->line [72]), &(rpr->line [72]), 8);

                        z = ((j - 1) & 0x007FFFFF) + 1;
                        memset (&(rpr->next->line [16]), 0, 56);
                        memcpy (&(rpr->next->line [16]), &(rpr->line [72 - z]), z);
                        memset (&(rpr->line [72 - z]), 0, z);
                        rpr->line [11] = (char)((j - i) - z);

                        rpr->next->line [4] = ((ocnt + 1) >> 1);
                        rpr->next->line [5] = (((ocnt + 1) & 1) << 7);
                        rpr->next->line [6] = 0;
                        rpr->next->line [7] = 0;
                        rpr->next->line [11] = (char)z;

                        j = i + rpr->line [11];
                    }
                } else {
                    rpr = rpr->next;
                    continue; /* Only output records within this 8MB */
                }

                z = 16 + rpr->line [11];

                rrp = rhead;
                while (rrp) {
                    if ((rrp->noffset) && (rrp->offset >= i) && (rrp->offset < j)) {

                        tl = sl;
                        l = 16 + (rrp->offset - i);
                        dl [0] = tl [l++];
                        if (l == z) {
                            l = 16;
                            tl = rpr->next->line;
                            dl [1] = tl [l++];
                            dl [2] = tl [l++];

                        } else {
                            dl [1] = tl [l++];
                            if (l == z) {
                                l = 16;
                                tl = rpr->next->line;
                                dl [2] = tl [l++];
                            } else {
                                dl [2] = tl [l++];
                                if (l == z) {
                                    l = 16;
                                    tl = rpr->next->line;
                                }
                            }
                        }
                        dl [3] = tl [l++];

                        k = *((long *)dl);
                        k = wordswap (k);

                        k += rrp->noffset;
                        k = wordswap (k);
                        *((long *)dl) = k;

                        tl = sl;
                        l = 16 + (rrp->offset - i);
                        tl [l++] = dl [0];
                        if (l == z) {
                            l = 16;
                            tl = rpr->next->line;
                            tl [l++] = dl [1];
                            tl [l++] = dl [2];

                        } else {
                            tl [l++] = dl [1];
                            if (l == z) {
                                l = 16;
                                tl = rpr->next->line;
                                tl [l++] = dl [2];
                            } else {
                                tl [l++] = dl [2];
                                if (l == z) {
                                    l = 16;
                                    tl = rpr->next->line;
                                }
                            }
                        }
                        sl [l++] = dl [3];
                    }

                    rrp = rrp->next;
                }

                rp = rlchead;
                while (rp) {

                    if ((rp->offset >= i) && (rp->offset < j) && (rp->xp) && (rp->xp->offset != -1)) {

                        new_adx (rp->offset, &rhead, &rtail, 0);

                        tl = sl;
                        l = 16 + (rp->offset - i);
                        dl [0] = tl [l++];
                        if (l == z) {
                            l = 16;
                            tl = rpr->next->line;
                            dl [1] = tl [l++];
                            dl [2] = tl [l++];

                        } else {
                            dl [1] = tl [l++];
                            if (l == z) {
                                l = 16;
                                tl = rpr->next->line;
                                dl [2] = tl [l++];
                            } else {
                                dl [2] = tl [l++];
                                if (l == z) {
                                    l = 16;
                                    tl = rpr->next->line;
                                }
                            }
                        }
                        dl [3] = tl [l++];

                        k = *((long *)dl);
                        k = wordswap (k);

                        k += rp->xp->offset;
                        k = wordswap (k);
                        *((long *)dl) = k;

                        tl = sl;
                        l = 16 + (rp->offset - i);
                        tl [l++] = dl [0];
                        if (l == z) {
                            l = 16;
                            tl = rpr->next->line;
                            tl [l++] = dl [1];
                            tl [l++] = dl [2];

                        } else {
                            tl [l++] = dl [1];
                            if (l == z) {
                                l = 16;
                                tl = rpr->next->line;
                                tl [l++] = dl [2];
                            } else {
                                tl [l++] = dl [2];
                                if (l == z) {
                                    l = 16;
                                    tl = rpr->next->line;
                                }
                            }
                        }
                        tl [l++] = dl [3];
                    }

                    rp = rp->next;
                }

                pp = phead;
                while (pp) {

                    if ((pp->offset >= i) && (pp->offset < j)) {

                        new_reloc (pp->offset, -1, 0);

                        tl = sl;
                        l = 16 + (pp->offset - i);
                        dl [0] = tl [l++];
                        if (l == z) {
                            l = 16;
                            tl = rpr->next->line;
                            dl [1] = tl [l++];
                            dl [2] = tl [l++];

                        } else {
                            dl [1] = tl [l++];
                            if (l == z) {
                                l = 16;
                                tl = rpr->next->line;
                                dl [2] = tl [l++];
                            } else {
                                dl [2] = tl [l++];
                                if (l == z) {
                                    l = 16;
                                    tl = rpr->next->line;
                                }
                            }
                        }
                        dl [3] = tl [l++];

                        k = *((long *)dl);
                        k = wordswap (k);

                        k += pp->object;
                        k = wordswap (k);
                        *((long *)dl) = k;

                        tl = sl;
                        l = 16 + (pp->offset - i);
                        tl [l++] = dl [0];
                        if (l == z) {
                            l = 16;
                            tl = rpr->next->line;
                            tl [l++] = dl [1];
                            tl [l++] = dl [2];

                        } else {
                            tl [l++] = dl [1];
                            if (l == z) {
                                l = 16;
                                tl = rpr->next->line;
                                tl [l++] = dl [2];
                            } else {
                                tl [l++] = dl [2];
                                if (l == z) {
                                    l = 16;
                                    tl = rpr->next->line;
                                }
                            }
                        }
                        sl [l++] = dl [3];
                    }

                    pp = pp->next;
                }

                memcpy (dl, sl, 72);

                dl [4] = 64;
                dl [5] &= 0x7F; /* 8MB */

                rsbuf ();
                memcpy (&(dl [72]), sbuf, 8);

                fwrite (dl, 1, 80, fho);

                rpr = rpr->next;
            }

            /* RLDs for new RENT data template */
            dl [0] = 2;
            dl [1] = 'R';
            dl [2] = 'L';
            dl [3] = 'D';
            atoe (&(dl [1]), 3);
            memset (&(dl [4]), 64, 6);

            k = 1;
            rp = rlchead;
            while (rp) {

                while ((rp) && (((rp->xp != NULL) && (rp->xp->type != ER)) ||
                       ((rp->offset >> 23) != ocnt))) rp = rp->next;

                if (rp) {
                    memset (&(dl [10]), 64, 62);
                    dl [10] = 0;

                    i = 16;
                    while ((rp) && (i + 4 + (k * 4) <= 72)) {

                        if (k) {
                            if (rp->xp) {
                                dl [i++] = (unsigned char)(rp->xp->id >> 8);
                                dl [i++] = (unsigned char)rp->xp->id;
                            } else {
                                dl [i++] = (unsigned char)0;
                                dl [i++] = (unsigned char)2;
                            }
                            dl [i++] = 0;
                            dl [i++] = 1;
                        }

                        if (rp->v)
                            dl [i++] = 0x1C;
                        else
                            dl [i++] = 0x0C;

                        sp = rp->next;
                        while ((sp) && (((sp->xp != NULL) && (sp->xp->type != ER)) ||
                               ((sp->offset >> 23) != ocnt))) sp = sp->next;

                        if ((sp) &&
                            (rp->xp == sp->xp) &&
                            (i + 3 + 4 <= 72)) {
                            dl [i-1] |= 1;
                            k = 0;
                        } else
                            k = 1;

                        dl [i++] = (rp->offset >> 16) & 0x7F;
                        dl [i++] = (rp->offset >> 8) & 0xFF;
                        dl [i++] = rp->offset & 0xFF;

                        rp = sp;
                    }

                    dl [11] = i - 16; /* length */

                    rsbuf ();
                    memcpy (&(dl [72]), sbuf, 8);

                    fwrite (dl, 1, 80, fho);
                }
            }

            ocnt++;

            dl [0] = 2;
            dl [1] = 'E';
            dl [2] = 'N';
            dl [3] = 'D';
            atoe (&(dl [1]), 3);
            memset (&(dl [4]), 64, 68);

            rsbuf ();
            memcpy (&(dl [72]), sbuf, 8);

            fwrite (dl, 1, 80, fho);

            sequence = 1;
        } while (mloop--);

        /* NEXT OBJECT - RUNTIME OBJECT TABLE */
        bend = 4 * o_cnt;

        dl [0] = 2;
        memcpy (&(dl [1]), xsdrec, 3);
        memset (&(dl [4]), 64, 6);

        len = 8;

        memset (&(dl [10]), 0, 22);

        dl [11] = (unsigned char)(0x10 + len);
        dl [15] = (unsigned char)1;
        dl [19] = (unsigned char)len;
        dl [23] = 1;

        dl [24] = (unsigned char)SD;

        dl [25] = 0;
        dl [26] = 0;
        dl [27] = 0;

        dl [28] = 0x07;
        dl [29] = (bend >> 16) & 0x7F;
        dl [30] = (bend >> 8) & 0xFF;
        dl [31] = bend & 0xFF;

        memset (&(dl [32 + len]), 64, 40 - len);
        memcpy (&(dl [32]), "@@JCCTBL", len);
        atoe (&(dl [32]), len);

        rsbuf ();
        memcpy (&(dl [72]), sbuf, 8);

        if (rename_sym != -1)
            fix_xsd_esd (dl);

        fwrite (dl, 1, 80, fho);

        /* Output TXT records for object offsets now: */
        dl [0] = 2;
        dl [1] = 'T';
        dl [2] = 'X';
        dl [3] = 'T';
        atoe (&(dl [1]), 3);
        dl [4] = 64;

        dl [8] = 64;
        dl [9] = 64;
        dl [10] = 0;
        dl [12] = 64;
        dl [13] = 64;
        dl [14] = 0;
        dl [15] = 1;

        o_offsets [0] = o_cnt; // The number of 'em... 1->1023
        i = o_cnt;
        j = 0;

        table_offset = 0;

        while (i > 0) {
            dl [5] = (table_offset >> 16) & 0x7F;
            dl [6] = (table_offset >> 8) & 0xFF;
            dl [7] = table_offset & 0xFF;

            if (i >= 14) {
                dl [11] = 56;
                l = 0;
                while (l < 14) {
                    k = o_offsets [j++];
                    k = wordswap (k);
                    memcpy (&(dl [16 + (l++ * 4)]), &k, 4);
                }
            } else {
                dl [11] = i * 4;
                l = 0;
                while (l < i) {
                    k = o_offsets [j++];
                    k = wordswap (k);
                    memcpy (&(dl [16 + (l++ * 4)]), &k, 4);
                }
                memset (&(dl [16 + (i * 4)]), 0, 56 - (i * 4));
            }

            rsbuf ();
            memcpy (&(dl [72]), sbuf, 8);

            fwrite (dl, 1, 80, fho);

            table_offset += 56;
            i -= 14;
        }

        dl [0] = 2;
        dl [1] = 'E';
        dl [2] = 'N';
        dl [3] = 'D';
        atoe (&(dl [1]), 3);
        memset (&(dl [4]), 64, 68);

        rsbuf ();
        memcpy (&(dl [72]), sbuf, 8);

        fwrite (dl, 1, 80, fho);

        sequence = 1;

        /* NEXT OBJECT - RUNTIME RELOCATIONS */
        k = 0;
        rrp = rhead;
        while (rrp) {
            k++;
            rrp = rrp->next;
        }

        dl [0] = 2;
        memcpy (&(dl [1]), xsdrec, 3);
        memset (&(dl [4]), 64, 6);

        len = 8;

        memset (&(dl [10]), 0, 22);

        dl [11] = (unsigned char)(0x10 + len);
        dl [15] = (unsigned char)1;
        dl [19] = (unsigned char)len;
        dl [23] = 1;

        dl [24] = (unsigned char)SD;

        dl [25] = 0;
        dl [26] = 0;
        dl [27] = 0;

        bend = 4 * (k + 1);

        dl [28] = 0x07;
        dl [29] = (bend >> 16) & 0xFF;
        dl [30] = (bend >> 8) & 0xFF;
        dl [31] = bend & 0xFF;

        memset (&(dl [32 + len]), 64, 40 - len);
        memcpy (&(dl [32]), "@@JCCRLD", len);
        atoe (&(dl [32]), len);

        rsbuf ();
        memcpy (&(dl [72]), sbuf, 8);

        if (rename_sym != -1)
            fix_xsd_esd (dl);

        fwrite (dl, 1, 80, fho);

        /* Output TXT records for relocations now: */
        dl [0] = 2;
        dl [1] = 'T';
        dl [2] = 'X';
        dl [3] = 'T';
        atoe (&(dl [1]), 3);
        dl [4] = 64;

        dl [8] = 64;
        dl [9] = 64;
        dl [10] = 0;
        dl [12] = 64;
        dl [13] = 64;
        dl [14] = 0;
        dl [15] = 1;

        table_offset = 0;

        i = 1;
        k = wordswap (k);
        memcpy (&(dl [16]), &k, 4);
        rrp = rhead;
        while (rrp) {
            k = rrp->offset;
            k = wordswap (k);
            memcpy (&(dl [16 + (i++ * 4)]), &k, 4);

            if (i == 14) {
                dl [5] = (table_offset >> 16) & 0xFF;
                dl [6] = (table_offset >> 8) & 0xFF;
                dl [7] = table_offset & 0xFF;

                dl [11] = 56;

                rsbuf ();
                memcpy (&(dl [72]), sbuf, 8);

                fwrite (dl, 1, 80, fho);

                table_offset += 56;
                i = 0;
            }

            rrp = rrp->next;
        }
        if (i) {
            dl [5] = (table_offset >> 16) & 0xFF;
            dl [6] = (table_offset >> 8) & 0xFF;
            dl [7] = table_offset & 0xFF;

            dl [11] = i * 4;

            memset (&(dl [16 + (i * 4)]), 0, 56 - (i * 4));

            rsbuf ();
            memcpy (&(dl [72]), sbuf, 8);

            fwrite (dl, 1, 80, fho);
        }

        dl [0] = 2;
        dl [1] = 'E';
        dl [2] = 'N';
        dl [3] = 'D';
        atoe (&(dl [1]), 3);
        memset (&(dl [4]), 64, 68);

        rsbuf ();
        memcpy (&(dl [72]), sbuf, 8);

        fwrite (dl, 1, 80, fho);
    }

    return (0);
}

void trav_includes () {
    fil_ptr fp;
    lnk_ptr lnk;
    long    i;
    lfl_ptr lfl;

    i = 1;
    while (i) {
        i = 0;

        fp = fhead;
        while (fp) {

            if (fp->include) {

                lnk = fp->head;
                while (lnk) {

                    if (lnk->symbols->file) {
                        if (lnk->symbols->file->include == 0) {

                            /* Only included if forced or not reported as missing/weak */
                            if ((lnk->symbols->reported == 0) || (forced)) {
                                lnk->symbols->file->include = 1;
                                i = 1;
                            } else
                            if (lnk->symbols->reported == -1) { /* might be needed, check the todo list */

                                lfl = lnk->symbols->uses;
                                while (lfl) {
                                    if (lfl->file->include) break;
                                    lfl = lfl->next;
                                }

                                if (lfl) { /* something is included, which doesn't have a weak external link */
                                    lnk->symbols->file->include = 1;
                                    i = 1;
                                }
                            }
                        }
                    } else {
                        if (lnk->symbols->reported == 0) {
                            if ((strcmp (lnk->symbols->name, "@@JCCRNT") != 0) &&
                                (strcmp (lnk->symbols->name, "@@JCCTBL") != 0) &&
                                (strcmp (lnk->symbols->name, "@@JCCRLD") != 0))
                            fprintf (stderr, "Symbol <%s> in file <%s> not found anywhere.\n", lnk->symbols->name, fp->file);
                            lnk->symbols->reported = 1;
                        } else
                        if (lnk->symbols->reported < 0) {

                            lfl = lnk->symbols->uses;
                            while (lfl) {
                                if (lfl->file->include) break;
                                lfl = lfl->next;
                            }

                            if (lfl) { /* something is included, which doesn't have a weak external link */
                                fprintf (stderr, "Symbol <%s> in file <%s> not found anywhere.\n", lnk->symbols->name, fp->file);
                                lnk->symbols->reported = 1;
                            }
                        }
                    }

                    lnk = lnk->next;
                }
            }
            fp = fp->next;
        }
    }
}

/*
Hex Value ESD Type Code:
    0x00  SD - Entry to object with Flags
    0x01  LD - Alternate entry
    0x02  ER - Required External
    0x04  PC - ? (Not supported) with Flags
    0x05  CM - ? (Not supported) with Flags
    0x06  XD(PR) ? (Not supported) with Alignment (flag)
    0x0A  WX - Weak External
*/

char * UTypes [3] = {"PC", "CM", "XD"};

void add_sym (fil_ptr fil, long type, char * name, long xsd) {
    sym_ptr sym;
    lnk_ptr lnk;
    lfl_ptr lfl;

    if ((type == 4) || (type == 5) || (type == 6)) {
        fprintf (stderr, "File <%s> contains unsupported ESD <%s> type %s.\n",
            fil->file, name, UTypes [type - 4]);
        errors++;
        exit (8);
    }

    sym = shead;
    while (sym) {
        if (strcmp (sym->name, name) == 0) break;
        sym = sym->next;
    }
    if (sym == NULL) {
        sym = (sym_ptr)malloc (sizeof (sym_t));
        sym->name = malloc (strlen (name) + 1);
        strcpy (sym->name, name);
        if (xsd) {
            sym->shortname = shortnames++;
            printf ("Mapping %-40s %s%06d\n", name, replace_prefix, sym->shortname);
        } else {
            sym->shortname = -1;
        }
        if (type == 0x0A) { /* Weak External? */
            sym->reported = 1; /* Don't produce warning messages */
        } else {
            sym->reported = 0;
        }
        sym->uses = NULL; /* For now... */
        sym->file = NULL; /* For now!  Might change just below */
        sym->next = shead;
        shead = sym;
    }
    if ((type == 0) || (type == 1)) { /* Defined in this file */

        if (sym->file != NULL) {
            fprintf (stderr, "Warning, <%s> is defined in multiple places,\n", name);
            fprintf (stderr, " symbol in %s will be used,\n", sym->file->file);
            fprintf (stderr, " symbol in %s will be discarded\n", fil->file);
        } else {
            sym->file = fil;
        }
    } else {                          /* Required by this file */
        lnk = (lnk_ptr)malloc (sizeof (lnk_t));
        lnk->symbols = sym;
        lnk->next = fil->head;
        fil->head = lnk;

        if ((type != 0x0A) && (sym->reported != 0)) { /* Not Weak External? && some other object has a weak external reference*/
            sym->reported = -1;                        /* Then reset this to a required external - maybe */

            lfl = (lfl_ptr)malloc (sizeof (lfl_t));
            lfl->file = fil;
            lfl->next = sym->uses;
            sym->uses = lfl;
        }
    }
}

int look (char * file, long add) {
    unsigned char buf1 [81];
    unsigned char buf2 [81];
    char   name [41];
    FILE * fh;
    char * nf;
    long   i;
    long   j;
    long   k;
    long   type;
    fil_ptr fil;

    nf = (char *)malloc (strlen (file) + 1);
    strcpy (nf, file);

    fil = (fil_ptr)malloc (sizeof (fil_t));
    fil->file = nf;
    fil->head = NULL;
    fil->include = add;
    fil->next = NULL;
    if (fhead == NULL) {
        fhead = fil;
    } else {
        ftail->next = fil;
    }
    ftail = fil;

    fh = fopen (file, "rb");
    if (fh != NULL) {
        while (fread (buf1, 1, 80, fh) == 80) {

            if (buf1 [0] == 2) { /* Object Record */

                if (strncmp (esdrec, &(buf1 [1]), 3) == 0) { /* ESD */

                    memcpy (buf2, buf1, 80);
                    etoa (buf2, 80);

                    i = byteswap (*((short *)(&buf1 [10])));
                    j = 16;
                    while (i > 0) { /* variable number of bytes in record */

                        strncpy (name, &(buf2 [j]), 8);
                        k = 8;
                        while ((k) && (name [k-1] == ' ')) k--;
                        name [k] = 0;
                        type = buf1 [j + 8]; /* Type */

                        if (rename_sym == 1) {
                            add_sym (fil, type, name, 1);
                        } else {
                            add_sym (fil, type, name, 0);
                        }

                        j += 16;
                        i -= 16;
                    }

                } else if (strncmp (xsdrec, &(buf1 [1]), 3) == 0) { /* XSD */

                    if ((buf1 [72] != plus) || (buf1 [24] != SD)) {

                        memcpy (buf2, buf1, 80);
                        etoa (buf2, 80);

                        i = wordswap (*((long *)(&buf1 [16]))); /* Name Length */

                        if (i > 40) {
                            strncpy (name, &(buf2 [32]), 40); /* Name */
                            name [40] = 0;
                            fprintf (stderr, "Name: <%s> too long by %d chars.\n", name, i);
                            errors++;
                            exit (8);
                        }

                        strncpy (name, &(buf2 [32]), i); /* Name */
                        name [i] = 0;
                        type = buf1 [24]; /* Type */

                        if (((i > 8) && (rename_sym != -1)) || (rename_sym == 1)) {
                            add_sym (fil, type, name, 1);
                        } else {
                            add_sym (fil, type, name, 0);
                        }
                    }
                }
            }
        }
        fclose (fh);
        return (0);
    }

    return (1);
}

#ifdef WIN32
 #define fileslash "\\"
#else
 #define fileslash "/"
#endif

static long startime;

void endit (void) {
    long i;
    char buf [260];

    i = clock ();
    if (CLOCKS_PER_SEC == 1000)
        sprintf (buf, "Total Time:%dms\n", (i - startime));
    else
        sprintf (buf, "Total Time:%dms\n", (int)((double)(i - startime) * (1000.0 / (double)CLOCKS_PER_SEC)));

    if (errors == 0)
        fprintf (stderr, "PLK-RC:0, %s\n", buf);
    else
        fprintf (stderr, "PLK-RC:8, %s\n", buf);
}

int main (int argc, char * argv []) {
    FILE * fh;
    FILE * fho;
    char   file [260];
    char   buf  [260];
    long   i;
    long   j = 0;
#ifdef JCC
    char * a;
#endif
    long   dolib = 1;

    startime = clock ();

    if (argc > 3) {
        if (strncmp (argv [1], "-test=", 6) == 0) {
            testout = fopen (&(argv [1][6]), "a");
            if (testout) fprintf (testout, "@LoadMap\n");
            j++;
        }

        if (strncmp (argv [1+j], "-pre=", 5) == 0) {
            j++;
            if (argv [j][5]) {     /* Is there a replacement for 'S'? */
                replace_prefix [0] = argv [j][5];
                if (argv [j][6]) { /* We leave 'T' if the user specifies only 1 replacement char */
                    replace_prefix [1] = argv [j][6];
                }
            }
        }

        if (stricmp (argv [1+j], "-r") == 0) { /* "forced" is now only available on your special builds as */
            j++;
            rename_sym = 1;
            /*if (argv [j][1] == 'R') forced = 1;*/ /* some OS's capitalise parms, so this has been removed */
        } else if (stricmp (argv [1+j], "-x") == 0) {
            j++;
            rename_sym = -1;
            /*if (argv [j][1] == 'X') forced = 1;*/ /* It was wrong anyway since 2003 as the check was for */
        } else if (stricmp (argv [1+j], "-s") == 0) {
            j++;
            /*if (argv [j][1] == 'S') forced = 1;*/ /* the next parameter (1+j) until this change was made. */
        } else if (strncmp (argv [1+j], "-", 1) == 0) {
            argc = 0; /* bad argument */
        }
    }

    if (argc <= 3 + j) {
        fprintf (stderr, "Usage: prelink -test=file -pre=ST -r|s|x libdir target.obj user1.obj...\n");
        fprintf (stderr, " All parameters are position specific and only the flags are optional,\n");

        fprintf (stderr, " -test=file appends debugging information to the JCC test file.\n");

        fprintf (stderr, " -pre=XX alters the default \"ST\" symbol prefix used on replacements.\n");

        fprintf (stderr, " -r renames all symbols to STnnnnnn to hide their meanings,\n");
        fprintf (stderr, " -s performs the default function of only renaming longnames,\n");
        fprintf (stderr, " -x performs no renames at all, and leaves XSD records as-is.\n");
        /*fprintf (stderr, "    CAPITALISATION: -{R|S|X} forces weak externals to be linked,\n");*/

        fprintf (stderr, " libdir must contain the file liblst.txt listing all objects.\n");
        fprintf (stderr, " Additionally, libdir may be specified as 'nolib' to exclude the library.\n");

        fprintf (stderr, " user1.obj may be replaced with -file.txt which lists all the user objects.\n");

        if (testout) fclose (testout);
        return (12);
    }

    atexit (endit);

    if (stricmp (argv [1 + j], "nolib") == 0) dolib = 0;

#ifndef JCC
    plus = __ascii_to_ebcdic ['+'];

    esdrec [0] = __ascii_to_ebcdic ['E'];
    esdrec [1] = __ascii_to_ebcdic ['S'];
    esdrec [2] = __ascii_to_ebcdic ['D'];

    xsdrec [0] = __ascii_to_ebcdic ['X'];
    xsdrec [1] = __ascii_to_ebcdic ['S'];
    xsdrec [2] = __ascii_to_ebcdic ['D'];

    fncrec [0] = __ascii_to_ebcdic ['F'];
    fncrec [1] = __ascii_to_ebcdic ['N'];
    fncrec [2] = __ascii_to_ebcdic ['C'];

    txtrec [0] = __ascii_to_ebcdic ['T'];
    txtrec [1] = __ascii_to_ebcdic ['X'];
    txtrec [2] = __ascii_to_ebcdic ['T'];

    prdrec [0] = __ascii_to_ebcdic ['P'];
    prdrec [1] = __ascii_to_ebcdic ['R'];
    prdrec [2] = __ascii_to_ebcdic ['D'];

    rrdrec [0] = __ascii_to_ebcdic ['R'];
    rrdrec [1] = __ascii_to_ebcdic ['R'];
    rrdrec [2] = __ascii_to_ebcdic ['D'];

    rntrec [0] = __ascii_to_ebcdic ['R'];
    rntrec [1] = __ascii_to_ebcdic ['N'];
    rntrec [2] = __ascii_to_ebcdic ['T'];

    rndrec [0] = __ascii_to_ebcdic ['R'];
    rndrec [1] = __ascii_to_ebcdic ['N'];
    rndrec [2] = __ascii_to_ebcdic ['D'];

    if (dolib) {
        sprintf (file, "%s" fileslash "crt0.obj", argv [1 + j]);

#else
    plus = '+';

    esdrec [0] = 'E';
    esdrec [1] = 'S';
    esdrec [2] = 'D';

    xsdrec [0] = 'X';
    xsdrec [1] = 'S';
    xsdrec [2] = 'D';
    
    fncrec [0] = 'F';
    fncrec [1] = 'N';
    fncrec [2] = 'C';

    txtrec [0] = 'T';
    txtrec [1] = 'X';
    txtrec [2] = 'T';
    
    prdrec [0] = 'P';
    prdrec [1] = 'R';
    prdrec [2] = 'D';

    rrdrec [0] = 'R';
    rrdrec [1] = 'R';
    rrdrec [2] = 'D';

    rntrec [0] = 'R';
    rntrec [1] = 'N';
    rntrec [2] = 'T';

    rndrec [0] = 'R';
    rndrec [1] = 'N';
    rndrec [2] = 'D';

    if (dolib) {
        sprintf (file, "%s(crt0)", argv [1 + j]);
        strupr (file);

#endif

        if (look (file, 1) != 0) {
            fprintf (stderr, "Can't find crt0.obj in the library\n");
            if (testout) fclose (testout);
            errors++;
            return (8);
        }

        if (testout) { /* Add: debug.obj too */
#ifndef JCC
            sprintf (file, "%s" fileslash "debug.obj", argv [1 + j]);
#else
            sprintf (file, "%s(debug)", argv [1 + j]);
            strupr (file);
#endif
            if (look (file, 1) != 0) {
                fprintf (stderr, "Can't find debug.obj in the library\n");
                fclose (testout);
                errors++;
                return (8);
            }
        }
    }

    fho = fopen (argv [2 + j], "wb");
    if (fho == NULL) {
        fprintf (stderr, "Unable to open %s target object\n", argv [2 + j]);
        if (testout) fclose (testout);
        return (1);
    }

    i = 3 + j;
    if (argv [i][0] == '-') {/* Special...  User-Objects are listed in an input file */
        fh = fopen (&(argv [i][1]), "r");
        if (fh) {
            while (fgets (buf, 259, fh) != NULL) {
                i = strlen (buf);
                while ((i) && ((buf [i-1] == '\n') || (buf [i-1] == '\r'))) i--;
                buf [i] = 0;

                if (look (buf, 1) != 0) {
                    fprintf (stderr, "Can't find %s user specified object\n", buf);
                    errors++;
                }
            }
        } else {
            fprintf (stderr, "Can't find %s input file\n", &(argv [i][1]));
            errors++;
        }
    } else {                 /* Default...  User-Objects are on the command line */
        while (i < argc) {
            if (look (argv [i], 1) != 0) {
                fprintf (stderr, "Can't find %s user specified object\n", argv [i]);
                errors++;
            }
            i++;
        }
    }

    if (dolib) {
#ifndef JCC
        sprintf (file, "%s" fileslash "liblst.txt", argv [1 + j]);
#else
        sprintf (file, "%s(liblst)", argv [1 + j]);
        strupr (file);
#endif

        fh = fopen (file, "r");
        if (fh != NULL) {
            while (fgets (buf, 259, fh) != NULL) {
                i = strlen (buf);
                while ((i) && ((buf [i-1] == '\n') || (buf [i-1] == '\r'))) i--;
                buf [i] = 0;
                if ((stricmp (buf, "crt0.obj") != 0) && (stricmp (buf, "debug.obj") != 0)) {
#ifndef JCC
                    sprintf (file, "%s" fileslash "%s", argv [1 + j], buf);
#else
                    buf [8] = 0;
                    a = strstr (buf, ".");
                    if (a) a [0] = 0;
                    sprintf (file, "%s(%s)", argv [1 + j], buf);
                    strupr (file);
#endif
                    if (look (file, 0) != 0) {
                        fprintf (stderr, "Can't find %s in the library\n", buf);
                        errors++;
                    }
                }
            }
            fclose (fh);
        } else {
            fprintf (stderr, "No library list file <liblst.txt> was found in libdir\n");
            errors++;
        }
    }

    trav_includes ();

    if (trav_output (fho) != 0) {
        fprintf (stderr, "There was a problem building the target object.\n");
        errors++;
    } else if (dolib) {
        if (rename_sym == 1)
            sprintf (buf, " ENTRY %s%-71s", replace_prefix, "000000");
        else
            sprintf (buf, " ENTRY %-73s", "@@CRT0");

        atoe (buf, 80);

        fwrite (buf, 1, 80, fho);
    }

    fclose (fho);

    if (testout) fclose (testout);

    if (errors == 0)
        return (0);
    else
        return (8);
}
