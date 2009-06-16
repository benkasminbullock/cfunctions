/* This is a Cfunctions (version 0.25) generated header file.
   Cfunctions is a free program for extracting headers from C files.
   Get Cfunctions from `http://www.kasmin.org/cfunctions/'. */

/* This file was generated with:
`cfunctions -inmb tags.c' */
#ifndef CFH_TAGS_H
#define CFH_TAGS_H

/* From `tags.c': */

#line 31 "tags.c"
enum tag_type {
  TAG_FUNCTION,

  

  TAG_STRUCT_UNION_ENUM,
  TAG_MACRO,
  TAG_ENUM_VAL,
  TAG_TYPEDEF,
  TAG_GLOBAL,
  N_TAG_TYPES
}
#line 46 "tags.c"
;

#line 58 "tags.c"
extern const char * tag_type_name[];
/* -t option */

#line 70 "tags.c"

#ifdef CFUNCTIONS_DEBUG
extern int tag_debug;
/* 
   Output an entry for a new file.
*/

#line 79 "tags.c"

#endif /* CFUNCTIONS_DEBUG */
void tag_new_file (const char * file_name );
/* 
   Make a tag.  The input tag may have all kinds of trailing things.
   This removes the trailing junk.
*/

#line 98 "tags.c"
void tag_make (unsigned char * tag , enum tag_type t , unsigned line );

#line 163 "tags.c"
void tag_init (void);

#line 170 "tags.c"
void tag_exit (void);

#endif /* CFH_TAGS_H */
