/*

  Assert C-Library Include File for MVS-OS/390-zOS

  (C)2003 Jason Paul Winter, All Rights Reserved.

*/

#ifndef __ASSERTH
#define __ASSERTH /* Prevent multiple includes */

#ifdef NDEBUG
 #define assert(ex)
#else
 #define assert(ex) __assert ((int)(ex), __FILE__, __LINE__)
#endif

void __assert (int ex, char * file, int line);

#endif
