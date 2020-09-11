/*

  Signal C-Library Include File for MVS-OS/390-zOS

  (C)2003 Jason Paul Winter, All Rights Reserved.

*/

#ifndef __SIGNALH
#define __SIGNALH /* Prevent multiple includes */

#define SIG_ERR -1 /* Error return           */
#define SIG_DFL ((void(*)(int))0)  /* Default action         */
#define SIG_IGN ((void(*)(int))1)  /* Ignore action          */

#define SIGINT  2  /* CTRL+C interrupt       */
#define SIGILL  4  /* Illegal instruction    */
#define SIGABRT 6  /* Abnormal Termination   */
#define SIGFPE  8  /* Floating-point error   */
#define SIGSEGV 11 /* Illegal storage access */
#define SIGTERM 15 /* Termination request    */

int raise (int s);
void(*signal(int sig, void(*func)(int sig)) ) (int sig);

#endif
