/* required headers, some are needed by the generated code too */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <process.h>

#include <aux3270.h>

extern int __libc_tso_status; /* Testing old code?  Need TSO for that... */

#include "test1.h" /* generated 3270 i/o panel */
/* (include as many as needed, but give them unique names) */

static char * trim (char * in) { /* removes trailing spaces */
    long i;

    i = strlen (in);
    while (i > 0) {
        if (in [--i] == ' ') {
            in [i] = 0;
        } else {
            break;
        }
    }
    return (in);
}

int tst_panel (void * a) {
    long   i;
    long   j;

    long   cursor; /* desired/recorded cursor i/o-list-entry location */

    char   source [45];  /* panel variables, + space for a null-terminator */
    char   member1 [9];  /* ... */
    char   dest   [45];  /* ... */
    char   member2 [9];  /* ... */
    char * iolist [4]; /* list of input/output variables */

    char   errormsg [41]; /* our output variable-label in the example */
    char * olist  [1]; /* list of output only variables */

    /* Show how to dynamically set attributes */
    unsigned char   erattr [2]; /* attribute override, for errormsg above */
    unsigned char * oattr  [1]; /* 1 slot needed for the error attribute. */

    unsigned char   s1attr [3]; /* In/Out's require 3 bytes each */
    unsigned char   m1attr [3]; /* where in-only (see above) requires 2. */
    unsigned char   d2attr [3];
    unsigned char   m2attr [3];
    unsigned char * ioattr [4]; /* 4 slots needed for io variables. */

    /* Required pointers to buffers for 3270 io */
    unsigned long * svcmem; /* 3270 control area */
    unsigned char * b;      /* 3270 stream buffer */

    //if (__libc_tso_status == 1)     /* IF TSO : Do you want to */
    //    SetNative (THIS_THREAD, 1); /* use the existing terminal? */

    while (panel_running == 1) {

        if (MYTERMON ()) {
            printf ("This program is being shutdown using an internal flag.\n");
            return (8);
        }

        panel_running = 2; /* Just one test... once disconnected, we end. */

        /* allocate required buffers */
        svcmem = (unsigned long *)__malloc24 (16);
        b = (unsigned char *)__malloc24 (4096);

        if ((svcmem == NULL) || (b == NULL)) {
            printf ("Not enough memory to allocate the required buffers\n");
            return (8);
        }

        /* set up the attribute overrides */
        oattr [0] = erattr;
        attr_init (erattr); /* Dont use oattr [0], no size is available */

        ioattr [0] = s1attr; /* A full list of overrides is needed even */
        ioattr [1] = m1attr; /* if you intend to only alter 1 in/out var. */
        ioattr [2] = d2attr;
        ioattr [3] = m2attr;
        attr_init (s1attr);
        attr_init (m1attr);
        attr_init (d2attr);
        attr_init (m2attr);

        /* set up the output variable */
        sprintf (errormsg, "Ready for input.");
        olist [0] = errormsg;

        /* set up the input / output variables */
        cursor = 0;
        iolist [0] = source;  /* (initially put cursor here: see above) */
        iolist [1] = member1;
        iolist [2] = dest;
        iolist [3] = member2;

        /* Start by changing the colour of the initial error message */
        attr_colour (erattr, attr_c_bwhite);

        /* set variable defaults (here we just clear the values) */
        /* a better solution is to read the last-used values from a file. */
        j = 0;
        while (j < 4) {
            iolist [j++][0] = 0;
        }

        /* run the panel in a loop */
        i = 0;
        while ((i != 3) && (i != -1)) { /* Wait for F3 to be pressed */

            /* Information... */
            /* olist, iolist or cursor may be NULL when */
            /* there are no definitions in the panel which use them, */
            /* oattr and ioattr may be NULL if you have no overrides. */
            i = test1 (b, svcmem, olist, iolist, &cursor, oattr, ioattr);

            /* 'condition' the input data */
            j = 0;
            while (j < 4) {
                trim (iolist [j]);   /* don't want trailing spaces */
                strupr (iolist [j]); /* but want uppercase values */
                j++;
            }

            /* do any required work based on input */
            if (i == 5) { /* F5 processes input */

                /* does nothing really! */
                sprintf (errormsg, "Copy %s to %s OK.", member1, member2);
                attr_colour (erattr, attr_c_bblue);
                attr_style  (erattr, attr_s_default);

                /* For an example of protecting something... */
                attr_protect (m1attr, attr_p_protected);
                attr_protect (m2attr, attr_p_protected);

            } else if (i != 3) { /* show the key number pressed */

                sprintf (errormsg, "%d pressed.", i);
                attr_colour (erattr, attr_c_default);
                attr_style  (erattr, attr_s_reverse);

                if (i == 4) { /* Hidden F4 unprotects again. */
                    attr_protect (m1attr, attr_p_unprotect);
                    attr_protect (m2attr, attr_p_unprotect);
                }
            }
        }

        /* deallocate buffers */

        free (svcmem);
        free (b);

        /* finished with the fullscreen terminal */
        MYTERMFF ();
    }

    return (0);
}

int can_panel (void * a) {
    Sleep (15000);     /* This thread sleeps for 15 seconds before */
    panel_running = 2; /* Telling the system that a connected */
    return (0);        /* user should no longer be re-connected. */
}

int main () {
    int i;
    int j;

    /* Start a thread for the cancel operation */

    //i = _beginthread (can_panel, 0, NULL);
    //tst_panel (NULL);

    /* OR - Start a thread for the new 3270 connection */

    //i = _beginthread (tst_panel, 0, NULL);
    //j = _beginthread (tst_panel, 0, NULL); /* Optional - start a 2nd */
    //Sleep (15000);
    //panel_running = 2; /* Remember: Comment out the assignment in tst_panel */
    
    /* AND - Wait for the thread to finish */

    //_syncthread (i);
    //_syncthread (j); /* Optional bit */

    /* OR ONLY - just do a simple test... */

    tst_panel (NULL);

    return (0);
}
