/*
 * JIS MVS3.8j WWW Application Server.  (C)2004 Jason Paul Winter, All Rights Reserved.
 *
 * jasonwinter@hotmail.com, use "Subject" wisely - spam gets deleted without being read.
 *
 */

#include <stdio.h>
#include <stdarg.h>

#define vberror 1
#include "asp.h"

// Error Wrapping Logic Object (Combined with Object Levels too.)

typedef struct _vb_error_tag * _vb_error_ptr;
typedef struct _vb_error_tag {
    jmp_buf       jmp_error;
    long          on_status; // Last status of on_error
    void        * R13;       // Stack (ie. function level)
    void        * aux;       // Used for syncing with Object-Stack
    _vb_error_ptr next;
} _vb_error_sz;

// Properties

static RESPONS     * Response;
static void        * find_ASP;
static _vb_error_ptr err_head = NULL;

// Methods

// Globals

static long on_error = 0;
static jmp_buf * jmp_error = NULL;

// Code

int VBError_init (void * me) {

#ifdef DEBUG
    printf ("VBError Created.\n");
#endif

    return (1);
};

int VBError_exit () {

#ifdef DEBUG
    printf ("VBError Deleted.\n");
#endif

    return (1);
};

/***********************************************************/
/*                                                         */
/***********************************************************/

void vb_setjmp () {

    find_ASP = err_head; // Set up a marker
};

void vb_pushjmp (void * aux) {
    _vb_error_ptr a;
    void       ** i;
    jmp_buf       jp;

    setjmp (jp); // Save *my* Registers!
    i = (void *)(jp [12]); // Get *my* R13
    if ((err_head != NULL) && (err_head->R13 == i [1])) return;

    a = (_vb_error_ptr)malloc (sizeof (_vb_error_sz));
    a->on_status = on_error;
    a->next = err_head;

    a->R13 = i [1]; // Last R13 (the caller)
    a->aux = aux;

    err_head = a;

    jmp_error = &(a->jmp_error);
    on_error = 0;
};

void * vb_popjmp () {
    _vb_error_ptr a;
    _vb_error_ptr b;
    void       ** i;
    jmp_buf       jp;
    void        * r = NULL;

    if (err_head != NULL) {

        setjmp (jp); // Save *my* Registers!
        i = (void *)(jp [12]); // Get *my* R13
        if (err_head->R13 != i [1]) {
            // Look for an i [1] entry, if so, delete all inclusive
            // if not: then assume it was called in error.

            a = err_head->next;
            while (a) {
                if (a->R13 == i [1]) break;
                a = a->next;
            };
            if (a) {
                while (err_head != a) {
                    b = err_head;
                    err_head = b->next;
                    if (b->aux) _pop_object (b->aux);
                    free (b);
                };
            } else {
                return (NULL);
            };
        };

        on_error = err_head->on_status;
        a = err_head;
        err_head = err_head->next;
        r = a->aux;
        free (a);
        if (err_head) {
            jmp_error = &(err_head->jmp_error);
        } else {
            jmp_error = NULL;
        };
    };

    return (r);
};

void * c_Wrap (void (*f)(), void * me) {
    long   i;
    long   j;
    void * a;
    void * r;
    long   z;

    a = _get_vector_r ();

    z = on_error;

    _push_object (me);
    vb_pushjmp (me);

    if (z) {
        on_error = 1;
        if (setjmp (*(jmp_error))) goto except;
    };

    r = _exec_vector (f, a);

    vb_popjmp ();
    _pop_object (me);
    return (r);

except:
    vb_popjmp ();
    _pop_object (me);
    
    if (on_error == 1)
        longjmp (*jmp_error, 1);

    return (0);
};

void vb_error (char * object, char * function, char * msg, ...) {
    va_list arg;
    char    m2 [512];
    long    i;
    _vb_error_ptr a;

    if (on_error == 1) { // Fix any known objects
        longjmp (*jmp_error, 1);
    };

    if (on_error == 2)
        return;

    sprintf (m2, "%s::%s - ", object, function);
    i = strlen (m2);

    va_start (arg, msg);
    vsnprintf (&(m2 [i]), 512 - i, msg, arg);
    va_end (arg);

    strcat (m2, "\n");

    (*(Response->Write)) (m2);

    while (err_head != find_ASP) {
        a = err_head;
        err_head = a->next;
        if (a->aux) _pop_object (a->aux);
        free (a);
    };
    jmp_error = &(err_head->jmp_error);
    longjmp (*jmp_error, 1);
};
