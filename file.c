/* Various useful file functions */

#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <errno.h>
#include <stdlib.h>

#include "file.h"
#include "error-msg.h"
#include "sys-or-exit.h"

static int file_n_mallocs;

#define USUAL_BLOCKS 0x1000

/* 
   Given a FILE * to write output to and an input file name, copy the
   contents of the input file into the output file. If the copy is
   unsuccessful, this exits with an error message.
*/

void
fcopy (FILE * out, const char * in_file_name)
{
    FILE * in_file;
    unsigned char copy[USUAL_BLOCKS];
    size_t a;
    size_t b;

    in_file = fopen_or_exit (in_file_name, "r");
    do {
	a = fread (copy, 1, USUAL_BLOCKS, in_file);
	b = fwrite (copy, 1, a, out);
	if (b != a) {
	    error ("Write error writing from '%s': %s",
		   in_file_name, strerror (errno));
	}
    }
    while (a == USUAL_BLOCKS);
    if (ferror (in_file)) {
	error ("Read error reading from '%s': %s",
	       in_file_name, strerror (errno));
    }
    fclose (in_file);
}

/*
    Compares files a and b with names a_name, b_name.

    Return value:

    Macro          Value            Meaning
    -----------------------------------------------------------------
    A_B_SAME_FILE  -5               Both filenames are the same
    NO_A_FILE      -2               File a does not exist
    NO_B_FILE      -1               File b does not exist
    A_B_SAME        0               Contents of a are identical to b
    A_B_DIFFERENT   1               Contents of a and b are different
*/

int fdiff (const char * a_name, const char * b_name)
{
    struct stat stats[2];
    const char * names[2];
    FILE * files[2];
    char * block[2];
    int i;
    unsigned comp;
    size_t size;

    names[0] = a_name;
    names[1] = b_name;

    if (strcmp (a_name, b_name) == 0) {
	fprintf (stderr,
		 "File names '%s' and '%s' refer to the same file.\n",
		 a_name, b_name);
	return A_B_SAME_FILE;
    }
    for (i = 0; i < 2; i++) {
	if (stat (names[i], & stats[i])) {
	    switch (errno) {
	    case ENOENT:
		if (i == 0) {
		    return NO_A_FILE;
		}
		else {
		    return NO_B_FILE;
		}
	    default:
		fprintf (stderr, "%s:%d: error from stat ('%s'): %s.\n",
			 __FILE__, __LINE__, names[i], strerror (errno));
		// exit ok
		exit (EXIT_FAILURE);
	    }
	}
    }
    /* If the two files have different sizes, then they cannot
       possibly be identical. */
    size = stats[0].st_size;
    if (size != stats[1].st_size) {
	return A_B_DIFFERENT;
    }
    for (i = 0; i < 2; i++) {
	int bytes_read;
	block[i] = malloc_or_exit (size);
	file_n_mallocs++;
	files[i] = fopen (names[i], "r");
	if (! files[i]) {
	    error ("error from fopen ('%s'): %s.\n",
		   names[i], strerror (errno));
	}
	bytes_read = fread (block[i], 1, size, files[i]);
	if (bytes_read != size) {
	    error ("fread (%s): short read %d bytes (expected %d)",
		   names[i], bytes_read, size);
	}
	fclose (files[i]);
    }
    comp = memcmp (block[0], block[1], size);
    for (i = 0; i < 2; i++) {
	CALLX (free_or_exit (block[i]));
	file_n_mallocs--;
    }
    if (comp != 0) {
	return A_B_DIFFERENT;
    }
    else {
	return A_B_SAME;
    }
}

/* Does a file exist or not? 

   Return value: 1 if it exists, 0 if not.

   Side effects: Calls `stat' for file. */

int
fexists (const char * file_name)
{
    struct stat dummy;
  
    /* test for file before overwriting it */
    if (stat (file_name, & dummy)) {
	if (errno != ENOENT) {
	    error ("could not stat %s: %s", file_name, strerror (errno));
	}
	return 0;
    }
    return 1;
}

void
file_memory_check ()
{
    if (file_n_mallocs != 0) {
	fprintf (stderr, "%s:%d: file_n_mallocs = %d.\n",
		 HERE, file_n_mallocs);
    }
}
