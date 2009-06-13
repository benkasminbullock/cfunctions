/* This is a Cfunctions (version 0.25) generated header file.
   Cfunctions is a free program for extracting headers from C files.
   Get Cfunctions from `url is not available now'. */

/* This file was generated with:
`cfunctions -inmb file-name.c' */
#ifndef CFH_FILE_NAME_H
#define CFH_FILE_NAME_H

/* From `file-name.c': */
/* 
   Given a file name, strip out all the directory information leaving
   just the file name.  

   Return value: a pointer to somewhere in `file_name'.  If there are
   no directories in `file_name', a pointer to `file_name'.
*/

#line 36 "file-name.c"
char * strip_dir (char * file_name );
/* 
   Check whether a string (file_name) ends in `.c'. 

   Return value: position of `.c' if present
                 NULL if .c not at end.
*/

#line 58 "file-name.c"
char * is_c_file (char * file_name );

#endif /* CFH_FILE_NAME_H */
