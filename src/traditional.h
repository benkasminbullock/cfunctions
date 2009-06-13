/* This is a Cfunctions (version 0.25) generated header file.
   Cfunctions is a free program for extracting headers from C files.
   Get Cfunctions from `url is not available now'. */

/* This file was generated with:
`cfunctions -inmb traditional.c' */
#ifndef CFH_TRADITIONAL_H
#define CFH_TRADITIONAL_H

/* From `traditional.c': */
/* Debugging flag for the functions of this file. */

#line 39 "traditional.c"
extern int trad_debug;
/* Deal with commas in argument lists. For example in

char a (b, c) char b, c; { ... }

the comma after the second `b' causes a call to this function. */

#line 86 "traditional.c"
void traditional_comma (void);
/* Semicolon in argument list causes a call to this function. */

#line 101 "traditional.c"
void traditional_next (void);
/* Save a C word which is either an argument type or an argument.  The
   context decides which one.  In the case of ambiguity it is
   temporarily made a type, and left to `traditional_next' to sort
   things out. */

#line 118 "traditional.c"
void traditional_save (char * text , unsigned length );
/* Print the type of the variable `text' to `f'. */

#line 130 "traditional.c"
void traditional_print_type (FILE * f , char * text );
/* Free all the memory allocated and reset the counters. */

#line 164 "traditional.c"
void traditional_reset (void);

#endif /* CFH_TRADITIONAL_H */
