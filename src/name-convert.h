/* This is a Cfunctions (version 0.25) generated header file.
   Cfunctions is a free program for extracting headers from C files.
   Get Cfunctions from `url is not available now'. */

/* This file was generated with:
`cfunctions -inmb name-convert.c' */
#ifndef CFH_NAME_CONVERT_H
#define CFH_NAME_CONVERT_H

/* From `name-convert.c': */
/* Given a "StringLikeThis", return "slt" in allocated memory. */

#line 30 "name-convert.c"
char * caps_to_initials (const char * caps );
/* Given a "string_like_this", return a "string like this" in
   allocated memory. */

#line 50 "name-convert.c"
char * uscores_to_spaces (const char * uscores );
/* Given a "  String liKe this ", return a "string_like_this" in
   allocated memory. */

#line 64 "name-convert.c"
char * spaces_to_uscores (const char * spaces );
/* Given a "  String liKe this ", return a "string-like-this" in
   allocated memory. */

#line 101 "name-convert.c"
char * spaces_to_hyphens (const char * spaces );
/* Given a StringLikeThis, return a string_like_this in allocated
   memory. 

   Bug: this assumes that _ is not the last character of `caps'. */

#line 117 "name-convert.c"
char * caps_to_uscores (const char * caps );
/* Given a string_like_this, return a StringLikeThis in allocated
   memory. */

#line 143 "name-convert.c"
char * uscores_to_caps (const char * uscores );
/* Return the upper-case version of a lower case string. */

#line 165 "name-convert.c"
char * lower_to_upper (const char * lower );

#endif /* CFH_NAME_CONVERT_H */
