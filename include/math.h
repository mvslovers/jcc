/*

  Math C-Library Include File for MVS-OS/390-zOS

  (C)2003 Jason Paul Winter, All Rights Reserved.

*/

#ifndef __MATHH
#define __MATHH /* Prevent multiple includes */

#define M_2PI  6.2831853071795864
#define M_PI   3.1415926535897932
#define M_HPI  1.5707963267948966
#define M_QPI  0.7853981633974483
#define M_iPI  0.3183098861837907 /* 1/pi */
#define M_i2PI 0.6366197723675813 /* 2/pi */

#ifndef HUGE_VAL
 extern double     __infinity;
 #define HUGE_VAL (__infinity)
#endif

/* Trigonometric functions */
double sin    (const double x);
double cos    (const double x);
double tan    (const double x);
double cot    (const double x);
double asin   (const double x);
double acos   (const double x);
double atan   (const double x);
double atan2  (const double x, const double y);

/* Hyperbolic functions */
double sinh   (const double x);
double cosh   (const double x);
double tanh   (const double x);

/* Exponential, logarithmic and power functions */
double exp    (const double x);
double log    (const double x);
double log10  (const double x);
double pow    (const double x, const double y);
double sqrt   (const double a);

/* Nearest integer, absolute value, and remainder functions */
double ceil   (const double x);
double floor  (const double x);
double fabs   (const double x);
double ldexp  (const double x, const int pw2);
double frexp  (const double x, int *pw2);
double modf   (const double x, double * y);
double fmod   (const double x, const double y);
double _copysign (const double x, const double y);
double _chgsign  (const double x);

#endif
