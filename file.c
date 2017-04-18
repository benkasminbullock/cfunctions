/* Various useful file functions */

#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <errno.h>
#include <stdlib.h>

#include "file.h"
#include "error-msg.h"

#define USUAL_BLOCKS 0x1000

/* 
   Given a FILE * to write output to and an input file name, copy the
   contents of the input file into the output file. 

   Return value: 0 if successful, -1 if failed.

   Side effects: writes to `out', opens and closes `in_file_name'.
*/

int fcopy (FILE * out, const char * in_file_name)
{
    FILE * in_file;
    unsigned char copy[USUAL_BLOCKS];
    size_t a;
    size_t b;

    in_file = fopen (in_file_name, "r");
    if (! in_file) {
	fprintf (stderr, "Open '%s' failed: %s\n",
		 in_file_name, strerror (errno));
	return -1;
    }
    do {
	a = fread (copy, 1, USUAL_BLOCKS, in_file);
	b = fwrite (copy, 1, a, out);
	if (b != a) {
	    fprintf (stderr, "Write error writing from '%s': %s.\n",
		     in_file_name, strerror (errno));
	    return -1;
	}
    }
    while (a == USUAL_BLOCKS);
    if (ferror (in_file)) {
	fprintf (stderr, "Read error reading from '%s': %s.\n",
		 in_file_name, strerror (errno));
	return -1;
    }
    fclose (in_file);
    return 0;
}

/*
    Compares files a and b with names a_name, b_name.

    Return value:
                negative         failure
                -5               both filenames are the same
                -4               if cannot read a
                -3               if cannot read b
                -2               if cannot open a
                -1               if cannot open b
                0                if a is identical to b
                positive integer if a and b are different
                
    Side effects: opens & closes files with names a_name and b_name.
*/

#define A_B_SAME_FILE -5
#define NO_A_FILE -2
#define NO_B_FILE -1
#define A_B_DIFFERENT 1

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
	block[i] = malloc (size);
	if (! block[i]) {
	    fprintf (stderr, "%s:%d: malloc (%d) failed: %s.\n",
		     __FILE__, __LINE__, size, strerror (errno));
	}
	files[i] = fopen (names[i], "r");
	if (! files[i]) {
	    fprintf (stderr, "%s:%d: error from fopen ('%s'): %s.\n",
		     __FILE__, __LINE__, names[i], strerror (errno));
	    // exit ok
	    exit (EXIT_FAILURE);
	}
	bytes_read = fread (block[i], 1, size, files[i]);
	if (bytes_read != size) {
	    fprintf (stderr, "%s:%d: fread (%s): short read %d bytes (expected %d)",
		     __FILE__, __LINE__, names[i], bytes_read, size);
	    // exit ok
	    exit (EXIT_FAILURE);
	}
	fclose (files[i]);
    }
    comp = memcmp (block[0], block[1], size);
    for (i = 0; i < 2; i++) {
	free (block[i]);
    }
    return comp;
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

