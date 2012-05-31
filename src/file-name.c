/* Various routines for checking file names. */

#include <string.h>
#include "file-name.h"

/* 
   Given a file name, strip out all the directory information leaving
   just the file name.  

   Return value: a pointer to somewhere in `file_name'.  If there are
   no directories in `file_name', a pointer to `file_name'.
*/

char *
strip_dir ( char * file_name )
{
  char * last_dir;

  last_dir = strrchr ( file_name, '/' );

  if ( ! last_dir )
    last_dir = file_name;
  else
    last_dir++;

  return last_dir;
}

/* 
   Check whether a string (file_name) ends in `.c'. 

   Return value: position of `.c' if present
                 NULL if .c not at end.
*/

char * 
is_c_file ( char * file_name )
{
    char * s;

    s = strstr (file_name, ".c");
    if (! s) {
        return 0;
    }
    if (s[2] == '\0') {
        return s;
    }
    else {
        return 0;
    }
}

