/* This is a Cfunctions (version 0.25) generated header file.
   Cfunctions is a free program for extracting headers from C files.
   Get Cfunctions from `http://www.kasmin.org/cfunctions/'. */

/* This file was generated with:
`cfunctions -inmb sys_or_exit.c' */
#ifndef CFH_SYS_OR_EXIT_H
#define CFH_SYS_OR_EXIT_H

/* From `sys_or_exit.c': */

#line 33 "sys_or_exit.c"
#include <stdio.h>
#include <sys/stat.h>

#line 40 "sys_or_exit.c"
void * malloc_or_exit (size_t s );

#line 52 "sys_or_exit.c"
void * calloc_or_exit (size_t s , size_t size );

#line 64 "sys_or_exit.c"
void * realloc_or_exit (void * v , size_t s );

#line 74 "sys_or_exit.c"
void fseek_or_exit (FILE * file , long offset , int whence );

#line 81 "sys_or_exit.c"
void fread_or_exit (void * buffer , size_t size , size_t nmemb , FILE * stream );

#line 97 "sys_or_exit.c"
void fwrite_or_exit (const void * buffer , size_t size , size_t nmemb , FILE * stream );

#line 104 "sys_or_exit.c"
FILE * fopen_or_exit (const char * path , const char * mode );

#line 124 "sys_or_exit.c"
void stat_or_exit (const char * file_name , struct stat * buf );

#endif /* CFH_SYS_OR_EXIT_H */
