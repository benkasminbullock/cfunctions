/* This is a Cfunctions (version 0.25) generated header file.
   Cfunctions is a free program for extracting headers from C files.
   Get Cfunctions from `url is not available now'. */

/* This file was generated with:
`cfunctions -inmb print_usage.c' */
#ifndef CFH_PRINT_USAGE_H
#define CFH_PRINT_USAGE_H

/* From `print_usage.c': */

#line 2 "print_usage.c"
#include <getopt.h>
/* The maximum number of characters printable on a line. */

#line 14 "print_usage.c"
extern unsigned error_msg_width;
/* Print a usage message on stderr describing the options
   available. The arrays `long_options' and `usage' must have the same
   number of members, n_options. 

   The struct `struct option' is defined in `getopt.h' which is part
   of the GNU C library.  */

#line 65 "print_usage.c"
void print_usage (unsigned n_options , struct option * long_options , const char ** usage );

#endif /* CFH_PRINT_USAGE_H */
