/* This is a Cfunctions (version 0.27) generated header file.
   Cfunctions is a free program for extracting headers from C files.
   Get Cfunctions from `http://cfunctions.sourceforge.net/'. */

/* This file was generated with:
`cfunctions -inmb argument.c' */
#ifndef CFH_ARGUMENT_H
#define CFH_ARGUMENT_H

/* From `argument.c': */

#line 30 "argument.c"
#include "tags.h"
struct type {
  unsigned char * name;
  struct type * next, * prev;
  unsigned line;              
} 
/* This structure can store the information about the type of an
   argument, for example "int", "void", etc. It is also used to store
   the names of arguments. So in a declaration like "int a, b", three
   types, one each for "int", "a", and "b", will be created. */

#line 45 "argument.c"
;
struct shared_type {
  struct type * t;
  int ref_count;
} 
/* Some argument types are shared between several arguments.  For
   example in `int i, j;', the type `int' is shared between "i" and
   "j".  This structure stores those relationships. */

#line 55 "argument.c"
;
struct arg {
  enum {TYPE, POINTER, SUFFIX, CPP_MUDDLE} parse_state; 
                              
  struct type * name;         
  struct shared_type * types; 
  struct type * pointers;     
  struct type * suffixes;     
  unsigned line;              
  struct arg * prev, * next;  
    
    int is_typedef;
} 
/* A complete function argument. */

#line 71 "argument.c"
;
/* Create a completely new "arg" structure. */

#line 82 "argument.c"
struct arg * arg_start (void);
/* Create a new "arg" with the same type as "a". */

#line 96 "argument.c"
struct arg * arg_share (struct arg * a );
/* Add another string to an argument. */

#line 146 "argument.c"
void arg_add (struct arg * a , const char * t , unsigned line );
/* For a particular argument, make the last-seen word the name
   of the argument. */

#line 206 "argument.c"
void arg_put_name (struct arg * a );
/* Free the memory associated with an argument. */

#line 273 "argument.c"
void arg_free (struct arg * a );
/* Print just one argument. */

#line 372 "argument.c"
void arg_fprint (FILE * f , struct arg * a );
/* Print all the arguments in a list of them.  This is used for
   printing a list of globals, so the `extern' prefix is always used
   here. */

#line 393 "argument.c"
void arg_fprint_all (FILE * f , struct arg * a , int do_extern );
/* Make all the tags for a particular argument. */

#line 413 "argument.c"
void arg_tag (struct arg * a , enum tag_type t );
/* If there is anything tagable in the type field of `a' then make it
   a tag.  This is for struct, union and enum name tagging only. */

#line 426 "argument.c"
void arg_tagable (struct arg * a );

#endif /* CFH_ARGUMENT_H */
