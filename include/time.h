/*

  Time C-Library Include File for MVS-OS/390-zOS

  (C)2003 Jason Paul Winter, All Rights Reserved.

*/

#ifndef __TIMEH
#define __TIMEH /* Prevent multiple includes */

#ifndef size_t
#define size_t unsigned int
#endif

#define time_t           long
#define clock_t unsigned long

#define timelocal mktime

/* Frequency of the clock ticks reported by clock().  */
#define CLOCKS_PER_SEC 1000
#define CLK_TCK        1000 /* obsolete version */

struct tm {
    int  tm_sec;    /* seconds after the minute [0-60] */
    int  tm_min;    /* minutes after the hour [0-59] */
    int  tm_hour;   /* hours since midnight [0-23] */
    int  tm_mday;   /* day of the month [1-31] */
    int  tm_mon;    /* months since January [0-11] */
    int  tm_year;   /* years since 1900 */
    int  tm_wday;   /* days since Sunday [0-6] */
    int  tm_yday;   /* days since January 1 [0-365] */
    int  tm_isdst;  /* Daylight Savings Time flag */
    long tm_gmtoff; /* offset from CUT in seconds */
    char *tm_zone;  /* timezone abbreviation */
};

struct timeval {
    long tv_sec;
    long tv_usec;
};

struct timezone {
    long tz_minuteswest;
    long tz_dsttime;
};

char      * asctime      (struct tm *);
char      * ctime        (const time_t *);
struct tm * gmtime       (const time_t *);
struct tm * gmtime_r     (const time_t *, struct tm *);
struct tm * localtime    (const time_t *);
struct tm * localtime_r  (const time_t *, struct tm *);
time_t      mktime       (struct tm *);
time_t      time         (time_t * timer);
int         gettimeofday (struct timeval *, struct timezone *);
double      difftime     (time_t, time_t);
clock_t     clock        (void);
size_t      strftime     (char *, size_t, const char *, const struct tm *);

#endif
