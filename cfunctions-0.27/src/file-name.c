/* Various routines for checking file names. */

/*
   Copyright (C) 1998  Ben K. Bullock

   Cfunctions is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   Cfunctions is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with Cfunctions; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

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

  s = strstr ( file_name, ".c" );
  if ( ! s )
    return 0;
  if ( s[2] == '\0' )
    return s;
  else
    return 0;
}

