/*

  Auxiliary 3270 C-Library Include File for MVS-OS/390-zOS

  (C)2009 Jason Paul Winter, All Rights Reserved.

*/

#ifndef __AUX3270H
#define __AUX3270H /* Prevent multiple includes */

/* "#define NOAUX3270 1" to use only the original routines [which are now part */
/* of the aux3270.obj library member, ie. No longer pre-link the TSO .obj code] */
/* before including this file.  That is the same as the old 3270 designer code. */

#define THIS_THREAD -1
/* This can be used as threadid in SetNative below: */

int SetNative (int threadid, int value);
/* SetNative selects which thread of a JCC program is
   selected to use the TSO terminal IO methods instead
   of going through the emulated terminal IO routines.

   When this module is linked, the default is no TSO.
 
   threadid may be 0 for the main program, or the handle
   value returned from _beginthread - but note that you
   must control access (using a critical section) to all
   emulated IO functions between TERM-ON and OFF before
   this function is used.  Once connected, -1 is returned
   if this function is called for the connected thread.

   A 'value' of 0 turns off the TSO option for a thread.
 */

extern int panel_port;    /* '3278' is the default port to listen on */

extern int panel_running; /* Set to 0 to shutdown right away, OR */ 
                          /* Set to 2 to shutdown once users have logged off */

/* New re-routing functions for terminal IO: */

long MYTERMONi (void);   /* Will wait until a 3270 client connects */
long MYTERMFFi (void);   /* Cleans up / disconnects a client */
long MYTERMOi  (void *); /* Sends 3270 stream output */
long MYTERMIi  (void *); /* Reads 3270 stream input */

/* Use these short (original) assembler routines now provided by JCC: */

long MYTERMON (void);
long MYTERMFF (void);
long MYTERMO  (void *);
long MYTERMI  (void *);

#ifndef NOAUX3270

/* Alter the original 3270 designer API commands to use the new ones available: */

#define MYTERMON MYTERMONi
#define MYTERMFF MYTERMFFi
#define MYTERMO  MYTERMOi
#define MYTERMI  MYTERMIi

#endif

/* JCC 3270-stream helper macros */

/* Macros to set properties */
#define attr_init(a) memset (a, 0x00, sizeof (a)); /* Use defaults */
#define attr_color(a, c) a [0] = c
#define attr_colour(a, c) a [0] = c
#define attr_style(a, c) a [1] = c
#define attr_protect(a, c) a [2] = c /* Only for in/out's */

/* Colour values */
#define attr_c_default    0x00
#define attr_c_bwhite     0xF7
#define attr_c_bcyan      0xF5
#define attr_c_byellow    0xF6
#define attr_c_bgreen     0xF4
#define attr_c_bpurple    0xF3
#define attr_c_bred       0xF2
#define attr_c_bblue      0xF1

/* Style values */
#define attr_s_default    0x00
#define attr_s_normal     0xF0
#define attr_s_underscore 0xF4
#define attr_s_blink      0xF1
#define attr_s_reverse    0xF2

/* Protection values */
#define attr_p_default    0x00
#define attr_p_unprotect  0x00
#define attr_p_protected  0x01

#endif
