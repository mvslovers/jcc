/*

  Set Jump C-Library Include File for MVS-OS/390-zOS

  (C)2003,2009 Jason Paul Winter, All Rights Reserved.

*/

#ifndef __SETJMPH
#define __SETJMPH /* Prevent multiple includes */

typedef long jmp_buf [14];   /* registers 1-14 */

int  setjmp  (jmp_buf);      /* returns 0 on a save, 1 or longjmp int value otherwise */
void longjmp (jmp_buf, int); /* int can be zero, but is converted to 1 when at setjmp */

int _setjmp_stae (jmp_buf jbs, char * sdwa104);
int _setjmp_canc (void);

#endif
