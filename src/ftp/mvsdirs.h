/*
 * Copyright (c)2003, 2009 Jason Paul Winter, All Rights Reserved.
 */

#define maxdevs 200
static long numdevs = 0;
static char devices [maxdevs][7];
static char units   [maxdevs][7];

/* This is a non UCB access version of obtaining DASD system info:
static long dirsupport () {
    long   i;
    FILE * fh;
    char   line [81];

    fh = fopen ("//DSN:SYS1.PARMLIB(VATLST00)", "rb");
    if (fh == NULL) {
        return (1);
    }

    while (fread (line, 1, 80, fh) != 0) {

        // Add a sanity check..
        if ((line [6] != ',') ||
            (line [8] != ',') ||
            (line[10] != ',') ||
            (line[19] != ',')) {
            continue; // ..forget this line and go on to the next.
        }

        strncpy (devices [numdevs], line, 6);
        i = 6;
        do {
            devices [numdevs][i--] = 0;
        } while (devices [numdevs][i] == ' ');

        strncpy (units [numdevs], &(line [11]), 8);
        i = 6;
        do {
            units [numdevs][i--] = 0;
        } while (units [numdevs][i] == ' ');

        if (++numdevs == maxdevs) break;
    }

    fclose (fh);

    return (0);
}*/

static char * disktype [] = {
    "UNKN",   // 0
    "2311",   // 1
    "2301",   // 2
    "2303",   // 3
    "2302",   // 4 - could be "9345" if (c [17] != 0)
    "2321",   // 5
    "2305-1", // 6
    "2305-2", // 7
    "2314",   // 8
    "3330",   // 9
    "3340",   // A
    "3350",   // B
    "3375",   // C
    "3330-1", // D
    "3380",   // E
    "3390"    // F
};

static void adddev (char * serial, char * model) {
    long   i;

    if (numdevs == maxdevs) return;

    strncpy (devices [numdevs], serial, 6);
    i = 6;
    do {
        devices [numdevs][i--] = 0;
    } while ((i) && (devices [numdevs][i] == ' '));

    strcpy (units [numdevs], model);

    numdevs++;
}

//extern int __libc_oarch; // Moved to ftpd.c

#define UCBSCAN 18 /* Internal JCC library function */
long LOWLEVIO (long func, long p1, long p2, long * savearea);

typedef struct ucbscan_tag {
    unsigned char parm [116]; /* Service parameters */
    unsigned char work [100]; /* Restart token */
    unsigned char ucbc [48];  /* Copy of UCB */
} t_ucbscan;

#define ucbinit(scan) memset ((scan)->work, 0, 100)

/* The following macro is not used here, but will return the device number: */
#define ucbnchr(dest, scan) {memcpy (dest, &((scan)->parm [36]), 4); dest [4] = 0;}

int ucbscan (t_ucbscan * scan) {
    long      savearea [10]; /* For Registers 2..11 */

    memset (scan->parm, 0, 116);

    scan->parm [0] = 1;    /* Version=1 */
    *((unsigned char **)&(scan->parm [4])) = &(scan->ucbc [0]);
    *((unsigned char **)&(scan->parm [12])) = &(scan->work [0]);
    scan->parm [96] = 32;  /* DASD */
    scan->parm [97] = 128; /* NONBASE=NO */
    scan->parm [98] = 16;  /* DEVNCHAR=(get) */

    return (LOWLEVIO (UCBSCAN, (long)scan, 2, savearea));
}

static long dirsupport () {
    char * cvt = *((char **)16);
    unsigned char * c;
    // For new style...
    t_ucbscan scan;
    // For old style...
    char * ucb;
    unsigned short b;
    char   serial [7];

    serial [6] = 0;
    if (__libc_oarch == 0) { // Is this an old O/S?
        // Old OS style of reading UCB
        ucb = *((char **)&(cvt [40]));
        b = *((short *)ucb);
        while (b != 0xFFFF) { // End of list?
            c = (unsigned char *)b;
            if ((c) && // Check if null?
                ((c [18] & 0x20) != 0) && // Is a Disk?
                ((c [3] & 0x80) != 0) && // Is Online?
                ((c [6] & 0x40) == 0)) { // Is Ready?
                memcpy (serial, &(c [28]),6);
                if ((c [19] != 0x04) || (c [17] == 0x00))
                    adddev (serial, disktype [c [19] & 0x0F]);
                else
                    adddev (serial, "9345");
            }
            ucb += 2; // Continue to the next in the list.
            b = *((short *)ucb);
        }
    } else {
        // New OS style of reading UCB
        // This version only does static devices <16MB
        /*c = *((unsigned char **)&(cvt [1252]));
        while (c) { // End of list?
            if (((c [18] & 0x20) != 0) && // Is a Disk?
                ((c [3] & 0x80) != 0) &&  // Is Online?
                ((c [6] & 0x40) == 0)) {  // Is Ready?
                memcpy (serial, &(c [28]),6);
                if ((c [19] != 0x04) || (c [17] == 0x00))
                    adddev (serial, disktype [c [19] & 0x0F]);
                else
                    adddev (serial, "9345");
            }
            c = *((unsigned char **)&(c [8]));
        }*/

        // This version does all DASD devices (Use JCC lib of 11/JAN/09)
        ucbinit (&scan); // Start at the first UCB
        c = scan.ucbc; // Get ready..
        while (ucbscan (&scan) == 0) {
            if (((c [18] & 0x20) != 0) && // Is a Disk? (Not needed!)
                ((c [3] & 0x80) != 0) &&  // Is Online?
                ((c [6] & 0x40) == 0)) {  // Is Ready?
                memcpy (serial, &(c [28]),6);
                if ((c [19] != 0x04) || (c [17] == 0x00))
                    adddev (serial, disktype [c [19] & 0x0F]);
                else
                    adddev (serial, "9345");
            }
        }
    }
    return (0);
}

typedef struct dirs_tag * dirs_tag_ptr;
typedef struct dirs_tag {
    char name [45];
    long date;
    long org;   /* 1 = PS, 2 = PO */
} dirs;

#define maxdirs 100

typedef struct _root * p_root;
typedef struct _root {
    long dircount;
    dirs directories [maxdirs];
    p_root next;
} t_root;

static p_root root = NULL; // Nothing in the list

//** Dir-Util function
static int getorg (char * name) { // Not found? rc=0
    p_root r = root; // Start point
    int i;
    while (r) {
        i = 0;
        while (i < r->dircount) {
            if (strcasecmp (r->directories [i].name, name) == 0)
                return (r->directories [i].org);
            i++;
        }
        r = r->next;
    }
    return (0);
}

static long _ftp_i = 1;
static char _ftp_ly [250]; // 1970-2220

static long leapdays (long year) {
    long i;
    long j;
    long k = year - 1970;

    if (_ftp_i) { // init?
        _ftp_i = 0; // not again!
        i = 0;
        while (i < 250) { // 250 years into the future!
            //if (1970 + i % 4 == 0)
            if (((1970 + i) & 3) == 0)
                _ftp_ly [i] = 1; // Is leap
            else
                _ftp_ly [i] = 0; // Not leap
            i++;
        }
    }

    i = 0;
    j = 0;
    while (j != k) {
        if (_ftp_ly [j]) {
        //if ((j + 1970) % 4 == 0) {
            i = i + 366;
        } else {
            i = i + 365;
        }
        j++;
    }

    return (i);
}

static long readvtoc (char * device, char * unit, p_root rt) { // r is the newroot
    long   i;
    char * s;
    FILE * fh;
    char str [40];
    unsigned char b [96];
    char * t; // trim!

    p_root c = rt; // Current
    while (c->next)
        c = c->next; // Find the correct place to add new entries
    if (c->dircount == maxdirs) return (0); // Out of memory previously

    sprintf (str, "rb,vtoc,unit=%s,volser=%s", unit, device);
    fh = fopen ("", str); // Open VTOC
    if (fh == NULL) return (1); // Nothing on this volume???

    while (fread (b, 1, 96, fh) == 96) {
        if ((b [44] != '1') ||
            ((b [82] != 0x40) && (b [82] != 0x02)))
            continue; // Skip if it's not a DSN, PS or PO
        t = c->directories [c->dircount].name;
        memcpy (t, b, 44);
        i = 44;
        do {
            t [i--] = 0;
        } while ((i) && (t [i] == ' '));

        if (b [82] == 0x40)
            c->directories [c->dircount].org = 1;
        else
            c->directories [c->dircount].org = 2;

        i = b [75]; /* Calc year */
        if (i >= 70) {
            i = i + 1900;
        } else {
            i = i + 2000;
        }
        i = leapdays (i);
        i += (((b [76] << 8) + b [77]) - 1); /* Add days */
        c->directories [c->dircount].date = i * 24 * 60 * 60; /* time=seconds past 1970 */

        (c->dircount)++;

        if (c->dircount == maxdirs) { // break;
            c->next = (p_root)malloc (sizeof (t_root));
            if (c->next == NULL)
                break;
            c = c->next;
            c->dircount = 0;
            c->next = NULL;
        }
    }
    fclose (fh);
    return (0);
}

#define maxdirent 3000
static long pdsecount;
static char pdsename [maxdirent][9];

#define endmark "\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF"
#define SKIP_MASK ((int) 0x1F)
#define ALIAS_MASK ((int) 0x80)

long readdir (char * pds) {
    long   i;
    long   j;
    FILE * fh;
    char   line [256];
    char   tstr [9];
    char * a;
    char * name;
    short  b;
    short  count;
    short  skip;
    long   quit;
    int    info_byte;
    short  l;

    pdsecount = 0;

    fh = fopen (pds, "rb,klen=0,lrecl=256,blksize=256,recfm=u,force");

    if (fh == NULL) {
        printf ("Error opening directory in %s\n", pds);
        return (1);
    }

    fread (&l, 1, 2, fh); /* Skip U length */

    quit = 0;
    while (fread (line, 1, 256, fh) == 256) {

        a = &(line [2]);
        b = ((short *)&(line [0])) [0];
        count = 2;
        while (count < b) {
            
            if (memcmp (a, endmark, 8) == 0) {
                quit = 1;
                break;
            }

            name = a;
            a += 8;

            i = (((int *)a) [0]) & 0xFFFFFF00;
            a += 3;

            info_byte = (int)(*a);
            a++;

            skip = (info_byte & SKIP_MASK) * 2;
            
            strncpy (tstr, name, 8);
            j = 7;
            while (tstr [j] == ' ') j--;
            tstr [++j] = 0;

            //if ((info_byte & ALIAS_MASK) == 0) {
            //    printf ("Member: %08X %s\n", i, tstr);
            //} else {
            //    printf ("Alias:  %08X %s\n", i, tstr);
            //}

            if (pdsecount == maxdirent) {
                quit = 1;
                break;
            } else
                strcpy (pdsename [pdsecount++], tstr);

            a += skip;

            count += (8 + 4 + skip);
        }

        if (quit) break;

        fread (&l, 1, 2, fh); /* Skip U length */
    }

    fclose (fh);
    return (0);
}
