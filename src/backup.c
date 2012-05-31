/* Backups for Cfunctions. */

#include <sys/types.h>
#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <errno.h>

#include "error-msg.h"
#include "backup.h"
#include "sys_or_exit.h"

/* If ARG is an unambiguous match for an element of the
   null-terminated array OPTLIST, return the index in OPTLIST
   of the matched element, else -1 if it does not match any element
   or -2 if it is ambiguous (is a prefix of more than one element).  */

static int
argmatch (const char *arg, const char *const *optlist)
{
  int i;			/* Temporary index in OPTLIST.  */
  size_t arglen;		/* Length of ARG.  */
  int matchind = -1;		/* Index of first nonexact match.  */
  int ambiguous = 0;		/* If nonzero, multiple nonexact match(es).  */

  arglen = strlen (arg);

  /* Test all elements for either exact match or abbreviated matches.  */
  for (i = 0; optlist[i]; i++)
    {
      if (!strncmp (optlist[i], arg, arglen))
	{
	  if (strlen (optlist[i]) == arglen)
	    /* Exact match found.  */
	    return i;
	  else if (matchind == -1)
	    /* First nonexact match found.  */
	    matchind = i;
	  else
	    /* Second nonexact match found.  */
	    ambiguous = 1;
	}
    }
  if (ambiguous)
    return -2;
  else
    return matchind;
}

#ifdef HEADER

/* When to make backup files. */

enum backup_type
{
  /* Never make backups. */
  none,

  /* Make simple backups of every file. */
  simple,

  /* Make numbered backups of files that already have numbered backups,
     and simple backups of the others. */
  numbered_existing,

  /* Make numbered backups of every file. */
  numbered
};

#endif /* HEADER */

static const enum backup_type backup_types[] =
{
  simple, simple, numbered_existing, numbered_existing, numbered, numbered
};

static const char *const backup_args [] =
{
  "never", "simple", "nil", "existing", "t", "numbered", 0
};

/* Which type of backup file names are generated. */

enum backup_type backup_type = none;

/* The extension added to file names to produce a simple (as opposed
   to numbered) backup file name. */

const char *simple_backup_suffix = "~";

/* Return the type of backup indicated by VERSION.
   Unique abbreviations are accepted. */

enum backup_type
get_version (char * version)
{
  int i;

  if (version == 0 || *version == 0)
    return numbered_existing;
  i = argmatch (version, backup_args);
  if (i >= 0)
    return backup_types[i];
  error ("%s %s `%s'", (i == -1) ? "invalid" : "ambiguous", 
         "version control type", version);
  return 0;
}

char *
basename (const char *name)
{
  const char *base = name;

  while (*name)
    {
      if (*name == '/')
        base = name + 1;
      ++name;
    }
  return (char *) base;
}

char *
dirname (const char * path)
{
  char *newpath;
  char *slash;
  int length;                   /* Length of result, not including NUL.  */

  slash = strrchr (path, '/');
  if (slash == 0)
    {
      /* File is in the current directory.  */
      path = ".";
      length = 1;
    }
  else
    {
      /* Remove any trailing slashes from the result.  */
      while (slash > path && *slash == '/')
        --slash;

      length = slash - path + 1;
    }
  newpath = malloc (length + 1);
  if (newpath == 0)
    return 0;
  strncpy (newpath, path, length);
  newpath[length] = 0;
  return newpath;
}

/* Return the newly-allocated concatenation of STR1 and STR2. */

static char *
concat (const char *str1, const char *str2)
{
  char *newstr;
  int str1_length = strlen (str1);

  newstr = malloc_or_exit (str1_length + strlen (str2) + 1);
  strcpy (newstr, str1);
  strcpy (newstr + str1_length, str2);
  return newstr;
}

/* Return a string, allocated with malloc, containing
   "FILE.~VERSION~".  Return 0 if out of memory. */

static char *
make_version_name (const char * file, int version)
{
  char *backup_name;

  backup_name = malloc_or_exit (strlen (file) + 16);
  sprintf (backup_name, "%s.~%d~", file, version);
  return backup_name;
}

/* If BACKUP is a numbered backup of BASE, return its version number;
   otherwise return 0.  BASE_LENGTH is the length of BASE.
   BASE should already have ".~" appended to it. */

static int
version_number (const char * base, const char * backup, int base_length)
{
  int version;
  const char *p;

  version = 0;
  if (!strncmp (base, backup, base_length) && isdigit (backup[base_length]))
    {
      for (p = &backup[base_length]; isdigit (*p); ++p)
	version = version * 10 + *p - '0';
      if (p[0] != '~' || p[1])
	version = 0;
    }
  return version;
}

/* Return the number of the highest-numbered backup file for file
   FILE in directory DIR.  If there are no numbered backups
   of FILE in DIR, or an error occurs reading DIR, return 0.
   FILE should already have ".~" appended to it. */

static int
max_backup_version (const char * file_name, const char * dir_name)
{
  DIR *dirp;
  struct dirent *dp;
  int highest_version;
  int this_version;
  size_t file_name_length;

  dirp = opendir (dir_name);
  if (!dirp)
    return 0;

  highest_version = 0;
  file_name_length = strlen (file_name);

  while ((dp = readdir (dirp)) != 0)
    {
      if ( (! dp->d_ino != 0) 
           || strlen (dp->d_name) <= file_name_length)
	continue;

      this_version = version_number (file_name, dp->d_name, file_name_length);
      if (this_version > highest_version)
	highest_version = this_version;
    }
  if (closedir (dirp))
    error ("problem closing directory `%s': %s", dir_name, strerror(errno));
  return highest_version;
}

/* Return the name of the new backup file for file FILE,
   allocated with malloc.  Return 0 if out of memory.
   FILE must not end with a '/' unless it is the root directory.
   Do not call this function if backup_type == none. */

char *
find_backup_file_name (char *file)
{
  char *dir;
  char *base_versions;
  int highest_backup;

  if (backup_type == simple)
    return concat (file, simple_backup_suffix);
  base_versions = concat (basename (file), ".~");
  if (base_versions == 0)
    return 0;
  dir = dirname (file);
  if (dir == 0)
    {
      free (base_versions);
      return 0;
    }
  highest_backup = max_backup_version (base_versions, dir);
  free (base_versions);
  free (dir);
  if (backup_type == numbered_existing && highest_backup == 0)
    return concat (file, simple_backup_suffix);
  return make_version_name (file, highest_backup + 1);
}

