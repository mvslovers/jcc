/*

  MVS Utils C-Library Include File for MVS-OS/390-zOS

  (C)2003,2009 Jason Paul Winter, All Rights Reserved.

*/

#ifndef __MVSUTILSH
#define __MVSUTILSH /* Prevent multiple includes */

int _CIB_STOP (void); // Returns 1 if the CIB chain contains a STOP command

int _testauth (void); // Returns 1 if authorised, else 0 is returned
int _modeset (int p);
// _modeset p values:
// ------------------
// 0 = KEY=ZERO
// 1 = KEY=NZERO (Use the TCB KEY)
// 2 = SUPER
// 3 = PROBLEM

int _setstax (long * bb);        // "long bb [2]" is required here
int _setstax_canc (long * bb);   // bb [0] == 0 on initialisation
                                 // bb [1] is for internal use
// If the attention key is pressed, bb [0] is incremented.

int _putline (char * text); // Output to terminal
char * _getline (void);         // Input from terminal (with "edit")

int _write2log (char * msg); // int returned is a combination of R0 & R15

int _write2op  (char * msg); // msgid is returned by this function
int _wri2dom   (int msgid);  // (This is used internally by _write2opr)

int _write2opr (char * msg, int maxlen, char ** result, int timeout_ms);
// _write2opr returns a copy of the ECB that may or may not be signalled.
// "char * result" (address passed above) needs to be 'free'd if non-NULL
// on return.  timeout_ms may be -1 for an infinite wait.

// These are RAW access routines, not for normal use:
int _wri2log (void * m); // only 126 chars, else truncated
int _wri2op  (void * m); // only 125 chars, else truncated
int _wri2opr (void * m); // only 121 chars, else truncated

#endif
