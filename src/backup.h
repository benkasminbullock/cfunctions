/* This is a Cfunctions (version 0.25) generated header file.
   Cfunctions is a free program for extracting headers from C files.
   Get Cfunctions from `http://www.kasmin.org/cfunctions/'. */

/* This file was generated with:
`cfunctions -inmb backup.c' */
#ifndef CFH_BACKUP_H
#define CFH_BACKUP_H

/* From `backup.c': */

#line 75 "backup.c"
enum backup_type {
  
  none,

  
  simple,

  
  numbered_existing,

  
  numbered
}/* When to make backup files. */

#line 93 "backup.c"
;
/* Which type of backup file names are generated. */

#line 109 "backup.c"
extern enum backup_type backup_type;
/* The extension added to file names to produce a simple (as opposed
   to numbered) backup file name. */

#line 114 "backup.c"
extern const char * simple_backup_suffix;
/* Return the type of backup indicated by VERSION.
   Unique abbreviations are accepted. */

#line 121 "backup.c"
enum backup_type get_version (char * version );

#line 136 "backup.c"
char * basename (const char *name );

#line 150 "backup.c"
char * dirname (const char * path );
/* Return the name of the new backup file for file FILE,
   allocated with malloc.  Return 0 if out of memory.
   FILE must not end with a '/' unless it is the root directory.
   Do not call this function if backup_type == none. */

#line 269 "backup.c"
char * find_backup_file_name (char *file );

#endif /* CFH_BACKUP_H */
