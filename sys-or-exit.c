/* System call with exit on failure. */

/* These <system_call>_or_exit functions all have the same type
   arguments in the same order as <system_call>, with no extra
   arguments.  They are not documented here since they are almost
   identical to the system call. */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>
/* For "unlink". */
#include <unistd.h>

#include "error-msg.h"
#include "sys-or-exit.h"

static int n_mallocs;

void * 
malloc_or_exit (size_t s)
{
    void * v;

    v = malloc (s);

    if (! v) {
	error ("out of memory");
    }
    n_mallocs++;
    return v;
}

void * 
calloc_or_exit (size_t s, size_t size)
{
    void * v;

    v = calloc (s, size);

    if (! v) {
	error ("out of memory");
    }
    n_mallocs++;
    return v;
}

void * 
realloc_or_exit (void * v, size_t s)
{
    if (! v) {
	n_mallocs++;
    }
    v = realloc (v, s);

    if (! v) {
	error ("out of memory");
    }
    return v;
}

void
fseek_or_exit (FILE * file, long offset, int whence)
{
    if (fseek (file, offset, whence) == -1) {
	error ("file seek failed: %s", strerror (errno));
    }
}

void
fread_or_exit (void * buffer, size_t size, size_t nmemb, FILE * stream)
{
    if (fread (buffer, size, nmemb, stream) != nmemb) {
	if (ferror (stream)) {
	    error ("file read failed: %s", strerror (errno));
	}
	else if (feof (stream)) {
	    error ("file read failed: unexpected end of file");
	}
	else {
	    bug (HERE, "problems with fread, feof or ferror");
	}
    }
}

void
fwrite_or_exit (const void * buffer, size_t size, size_t nmemb, FILE * stream)
{
    if (fwrite (buffer, size, nmemb, stream) != nmemb) {
	error ("file write failed: %s", strerror (errno));
    }
}

FILE *
fopen_or_exit (const char * path, const char * mode)
{
    FILE * file;

    file = fopen (path, mode);

    if (! file) {
	error ("could not open file \"%s\": %s",
	       path, strerror (errno));
    }
    return file;
}

void
stat_or_exit (const char * file_name, struct stat * buf)
{
    if (stat (file_name, buf) == -1) {
	error ("stat failed on %s: %s", file_name, strerror (errno));
    }
}

void
fclose_or_exit (FILE * f)
{
    int status;
    status = fclose (f);
    if (status != 0) {
	error ("fclose failed: %s", strerror (errno));
    }
}

or_exit_t
free_or_exit (void * memory)
{
    if (! memory) {
	fprintf (stderr, "%s:%d: Attempt to free null pointer.\n",
		 __FILE__, __LINE__);
	return or_exit_null_pointer;
    }
    free (memory);
    n_mallocs--;
    return or_exit_ok;
}

char *
strdup_or_exit (const char * todup)
{
    char * dupped;
    dupped = strdup (todup);
    if (! dupped) {
	error ("strdup failed: %s", strerror (errno));
    }
    n_mallocs++;
    return dupped;
}

void
chmod_or_exit (const char * path, mode_t mode)
{
    int status;
    status = chmod (path, mode);
    if (status) {
	error ("Could not chmod (%s, %o): %s", path, mode, strerror (errno));
    }
}

void
unlink_or_exit (const char * path)
{
    int status;
    status = unlink (path);
    if (status) {
	error ("Could not unlink (%s): %s", path, strerror (errno));
    }
}

/* Check for memory leaks. */

void
memory_check ()
{
    if (n_mallocs != 0) {
	fprintf (stderr, "%s:%d: n_mallocs = %d\n",
		 __FILE__, __LINE__, n_mallocs);
    }
}

