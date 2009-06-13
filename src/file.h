/* This is a Cfunctions (version 0.25) generated header file.
   Cfunctions is a free program for extracting headers from C files.
   Get Cfunctions from `url is not available now'. */

/* This file was generated with:
`cfunctions -inmb file.c' */
#ifndef CFH_FILE_H
#define CFH_FILE_H

/* From `file.c': */
/* 
   Given a FILE * to write output to and an input file name, copy the
   contents of the input file into the output file. 

   Return value: 0 if successful, -1 if failed.

   Side effects: writes to `out', opens and closes `in_file_name'.
*/

#line 41 "file.c"
int fcopy (FILE * out , const char * in_file_name );
/* This might as well just exit on error rather than give all the
   different codes. */

#line 81 "file.c"
int fdiff (const char * a_name , const char * b_name );
/* Does a file exist or not? 

   Return value: 1 if it exists, 0 if not.

   Side effects: Calls `stat' for file. */

#line 145 "file.c"
int fexists (const char * file_name );
/* Get the size of a file. */

#line 162 "file.c"
unsigned fsize (const char * file_name );
/* Reopen stdin to a new file.  Dies on failure. */

#line 179 "file.c"
void freopen_stdin (const char * file_name );
/* Reopen stdout to a new file.  Dies on failure. */

#line 188 "file.c"
void freopen_stdout (const char * file_name );

#endif /* CFH_FILE_H */
