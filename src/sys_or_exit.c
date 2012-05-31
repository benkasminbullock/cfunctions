/* System call with exit on failure. */

/* These <system_call>_or_exit functions all have the same type
   arguments in the same order as <system_call>, with no extra
   arguments.  They are not documented here since they are almost
   identical to the system call. */

#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "error-msg.h"
#include "sys_or_exit.h"

#ifdef HEADER
#include <stdio.h>
#include <sys/stat.h>
#endif /* HEADER */

void * 
malloc_or_exit ( size_t s )
{
  void * v;

  v = malloc ( s );

  if ( ! v )
    error ( "out of memory" );
  return v;
}

void * 
calloc_or_exit ( size_t s, size_t size )
{
  void * v;

  v = calloc ( s, size );

  if ( ! v )
    error ( "out of memory" );
  return v;
}

void * 
realloc_or_exit ( void * v, size_t s )
{
  v = realloc ( v, s );

  if ( ! v )
    error ( "out of memory" );
  return v;
}

void
fseek_or_exit ( FILE * file, long offset, int whence )
{
  if ( fseek ( file, offset, whence ) == -1 )
    error ( "file seek failed: %s", strerror (errno));
}

void
fread_or_exit ( void * buffer, size_t size, size_t nmemb, FILE * stream )
{
  /* note that clearerr has been called by fopen_or_exit already. */

  if ( fread ( buffer, size, nmemb, stream ) != nmemb )
  {
    if ( ferror ( stream ) )
      error ( "file read failed: %s", strerror (errno) );
    else if ( feof ( stream ) )
      error ( "file read failed: unexpected end of file" );
    else
      bug ( HERE, "problems with fread, feof or ferror");
  }
}

void
fwrite_or_exit (const void * buffer, size_t size, size_t nmemb, FILE * stream )
{
  if ( fwrite ( buffer, size, nmemb, stream ) != nmemb )
    error ( "file write failed: %s", strerror (errno) );
}

FILE *
fopen_or_exit (const char * path, const char * mode)
{
  FILE * file;

  file = fopen ( path, mode );

  if ( ! file )
    error ( "could not open file \"%s\": %s",
            path, strerror ( errno ) );

  /* I cannot find in the documentation I have (linux man pages and
  K&R) whether clearerr needs to be called here or not.  I am
  including this although wonder if it is really necessary. */

  /* clearerr ( file ); */

  return file;
}

void
stat_or_exit (const char * file_name, struct stat * buf )
{
  if ( stat ( file_name, buf ) == -1 )
    error ( "stat failed on %s: %s", file_name, strerror ( errno ) );
}

