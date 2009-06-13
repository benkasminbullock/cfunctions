/* This is a Cfunctions (version 0.25) generated header file.
   Cfunctions is a free program for extracting headers from C files.
   Get Cfunctions from `url is not available now'. */

/* This file was generated with:
`cfunctions -inmb c-word.c' */
#ifndef CFH_C_WORD_H
#define CFH_C_WORD_H

/* From `c-word.c': */
/* 
   Determine the length of a word that could be a C word. 

   Return value: number of chars from start of x that could be a C
   word.
*/

#line 32 "c-word.c"
unsigned c_word (unsigned char * x );
/* This table of reserved words is taken from the file `c-parse.gperf'
   of GNU C 2.8.0. */

#line 139 "c-word.c"
extern const char * gnu_c_keywords[];

#line 141 "c-word.c"
extern const unsigned n_gnu_c_keywords;
/* 
   Given a string, see if it is a GNU C keyword.

   Return value: the number of the keyword in the `gnu_c_keywords'
   array if it is a keyword or -1 if not.

   Deficiency: uses binary search.  Could be much faster.  This is
   only for debugging though.  
*/

#line 155 "c-word.c"
int is_keyword (unsigned char * may_be_keyword );

#endif /* CFH_C_WORD_H */
