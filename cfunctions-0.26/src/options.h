/* This is a Cfunctions (version 0.25) generated header file.
   Cfunctions is a free program for extracting headers from C files.
   Get Cfunctions from `http://www.kasmin.org/cfunctions/'. */

/* This file was generated with:
`cfunctions -inmb options.c' */
#ifndef CFH_OPTIONS_H
#define CFH_OPTIONS_H

/* From `options.c': */

#line 22 "options.c"
#include <getopt.h>

#line 68 "options.c"
extern struct option long_options[];

#line 72 "options.c"
extern int n_options;

#line 112 "options.c"
extern const char * usage[];
/* 
   Extract a character string from a `struct option' (from GNU getopt)
   suitable for use with `getopt_long'.  

   Return value: allocated memory containing an option string.
*/

#line 123 "options.c"
char * short_options (struct option * long_options , unsigned n_options );

#endif /* CFH_OPTIONS_H */
