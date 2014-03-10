/* This is a Cfunctions (version 0.28) generated header file.
   Cfunctions is a free program for extracting headers from C files.
   Get Cfunctions from 'http://www.lemoda.net/cfunctions/'. */

/* This file was generated with:
'cfunctions -inmb controls.c' */
#ifndef CFH_CONTROLS_H
#define CFH_CONTROLS_H

/* From 'controls.c': */

#line 5 "controls.c"
struct warning {
  unsigned implicit_int      : 1;
  unsigned strict_prototypes : 1;
  unsigned reserved_words    : 1;
} 
/* Warnings. */

#line 11 "controls.c"
;
#define DBMSG(format,msg...) do {                               \
        printf ("%s:%d [%s]: ", __FILE__, __LINE__, __func__);   \
        printf (format, ## msg);                                \
    } while (0)

#line 18 "controls.c"
extern struct warning warns;

#endif /* CFH_CONTROLS_H */
