/* This is a Cfunctions (version 0.25) generated header file.
   Cfunctions is a free program for extracting headers from C files.
   Get Cfunctions from `http://www.kasmin.org/cfunctions/'. */

/* This file was generated with:
`cfunctions -inmb cpp.c' */
#ifndef CFH_CPP_H
#define CFH_CPP_H

/* From `cpp.c': */
/* Add arguments to the C preprocessor from a list. */

#line 73 "cpp.c"

#ifdef DISABLE_CPP

#else
void cpp_add_argument (const char * argument );
/* Start the C preprocessor on the named file and return a file
   pointer which is the pipe output.  */

#line 93 "cpp.c"
FILE * cpp_start (char * file_name );
/* Close the file to the preprocessor.  The return value is the status
   returned by `waitpid'. */

#line 128 "cpp.c"
int cpp_stop (FILE * f );

#endif /* ! defined (DISABLE_CPP) */

#endif /* CFH_CPP_H */
