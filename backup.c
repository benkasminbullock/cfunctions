/* This file contains code which makes backups for Cfunctions using an
   Emacs-like numbered version system. */

#include <sys/types.h>
#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <stdlib.h>
#include <ctype.h>
#include <errno.h>
#include <sys/stat.h>

#include "error-msg.h"
#include "backup.h"
#include "sys-or-exit.h"

static int backup_n_mallocs;

static char *
basename (const char *name)
{
    const char *base = name;

    while (*name) {
	if (*name == '/') {
	    base = name + 1;
	}
	name++;
    }
    return (char *) base;
}

static char *
dirname (const char * path)
{
    char *newpath;
    char *slash;
    int length;                   /* Length of result, not including NUL.  */

    slash = strrchr (path, '/');
    if (slash == 0) {
	/* File is in the current directory.  */
	path = ".";
	length = 1;
    }
    else {
	/* Remove any trailing slashes from the result.  */
	while (slash > path && *slash == '/') {
	    slash--;
	}
	length = slash - path + 1;
    }
    newpath = malloc_or_exit (length + 1);
    backup_n_mallocs++;
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
    backup_n_mallocs++;
    strcpy (newstr, str1);
    strcpy (newstr + str1_length, str2);
    return newstr;
}

/* Return an allocated string containing "file.~version~". */

static char *
make_version_name (const char * file, int version)
{
    char * backup_name;
    int maxlen;

    maxlen = strlen (file) + 16;
    backup_name = malloc_or_exit (maxlen);
    backup_n_mallocs++;
    snprintf (backup_name, maxlen, "%s.~%d~", file, version);
    return backup_name;
}

/* If "backup" is a numbered backup of "base", return its version
   number; otherwise return 0.  "base_length" is the length of "base".
   "base" should have ".~" appended to it. */

static int
version_number (const char * base, const char * backup, int base_length)
{
    int version;
    const char *p;

    version = 0;
    if (!strncmp (base, backup, base_length) && isdigit (backup[base_length])) {
	for (p = &backup[base_length]; isdigit (*p); p++) {
	    version = version * 10 + *p - '0';
	}
	if (p[0] != '~' || p[1]) {
	    version = 0;
	}
    }
    return version;
}

/* Return the number of the highest-numbered backup file for file
   "file_name" in directory "dir_name".  If there are no numbered
   backups of "file_name" in "dir_name", or if an error occurs reading
   "dir_name", return 0.  "file_name" should already have ".~"
   appended to it. */

static int
max_backup_version (const char * file_name, const char * dir_name)
{
    DIR *dirp;
    struct dirent *dp;
    int highest_version;
    int this_version;
    size_t file_name_length;

    dirp = opendir (dir_name);
    if (!dirp) {
	return 0;
    }
    highest_version = 0;
    file_name_length = strlen (file_name);

    while ((dp = readdir (dirp)) != 0) {
	if ( (! dp->d_ino != 0) 
	     || strlen (dp->d_name) <= file_name_length) {
	    continue;
	}
	this_version = version_number (file_name, dp->d_name, file_name_length);
	if (this_version > highest_version) {
	    highest_version = this_version;
	}
    }
    if (closedir (dirp)) {
	error ("problem closing directory `%s': %s", dir_name, strerror (errno));
    }
    return highest_version;
}

/* Return the name of the new backup file for "file". The name should
   be freed with "free_backup_name".  */

char *
find_backup_file_name (char *file)
{
    char *dir;
    char *base_versions;
    int highest_backup;

    base_versions = concat (basename (file), ".~");
    dir = dirname (file);
    if (dir == 0) {
	fprintf (stderr, "%s:%d: failed to make dirname.\n",
		 __FILE__, __LINE__);
	// exit ok
	exit (EXIT_FAILURE);
    }
    highest_backup = max_backup_version (base_versions, dir);
    CALLX (free_or_exit (base_versions));
    backup_n_mallocs--;
    CALLX (free_or_exit (dir));
    backup_n_mallocs--;
    return make_version_name (file, highest_backup + 1);
}

/* Free the memory used to store the backup name. */

void
free_backup_name (char * backup_name)
{
    CALLX (free_or_exit (backup_name));
    backup_n_mallocs--;
}

/* Check this file is not leaking memory. */

void
backup_memory_check ()
{
    if (backup_n_mallocs != 0) {
	fprintf (stderr, "%s:%d: backup_n_mallocs = %d.\n",
		 HERE, backup_n_mallocs);
    }
}
