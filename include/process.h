/*

  Process C-Library Include File for MVS-OS/390-zOS

  (C)2003, 2009 Jason Paul Winter, All Rights Reserved.

*/

#ifndef __PROCESSH
#define __PROCESSH /* Prevent multiple includes */

#define CRITICAL_SECTION   long
#define LPCRITICAL_SECTION long *

#define WAIT   0
#define NOWAIT 1

void    InitialiseCriticalSection (LPCRITICAL_SECTION lock);
#define InitializeCriticalSection InitialiseCriticalSection

void    EnterCriticalSection (LPCRITICAL_SECTION lock);
void    LeaveCriticalSection (LPCRITICAL_SECTION lock);

long    beginthread (int(*start_address)(void *), unsigned, void *);
#define _beginthread beginthread

int threadstatus (long threadid, long mode); // mode 0=stop 1=start
#define _threadstatus threadstatus

int threadpriority (long threadid, long value); // value +/- current
#define _threadpriority threadpriority

void    endthread (int);
#define _endthread endthread

int     syncthread (long);
#define _syncthread syncthread

void    Sleep (long);

int     system    (char *);
int     systemTSO (char *);

int     spawn  (int, char *, ...);
int     spawnc (int, char *);

// New for JCC 2009 - Events and Wait functions:

typedef struct event_tag * EVENT;

#define INFINITE -1
#define WAIT_OBJECT_0 0
#define WAIT_TIMEOUT -2
#define WAIT_FAILED -1

EVENT CreateEvent (int initial_state);

int   ResetEvent (EVENT e);
int   SetEvent (EVENT e);
int   EventStatus (EVENT e);

int   WaitForSingleEvent (EVENT e, int ms);
int   WaitForMultipleEvents (int Count, EVENT * arr, int WaitAll, int ms);

int   CloseEvent (EVENT e);

#endif
