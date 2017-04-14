/* Various useful file functions */

#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <errno.h>

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

    in_file = fopen (in_file_name, "r");
    if (in_file) {
	unsigned char copy[USUAL_BLOCKS];
	size_t a;
	size_t b;
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
    return -1;
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

/* This might as well just exit on error rather than give all the
   different codes. */

int fdiff (const char * a_name, const char * b_name)
{
    FILE * a, * b;

    char a_block[USUAL_BLOCKS], b_block[USUAL_BLOCKS];
    unsigned a_len, b_len, i, comp;

    if (strcmp (a_name, b_name) == 0) {
	return -5;
    }
    if (! (a = fopen (a_name, "r"))) {
	return -2;
    }
    if (! (b = fopen (b_name, "r"))) {
	fclose (a);
	return -1;
    }
    while (1) {
	a_len = fread (a_block, 1, USUAL_BLOCKS, a);
	if (a_len == 0) {
	    if (! feof (a)) {
		comp = -4;
		goto end;
	    }
	}
	b_len = fread (b_block, 1, USUAL_BLOCKS, b);
	if (b_len == 0) {
	    if (! feof (b)) {
		comp = -3;
		goto end;
	    }
	}
	if (a_len != b_len) {
	    comp = 1;
	    goto end;
        }
	for (i = 0; i < a_len; i++) {
	    if (a_block[i] != b_block[i]) {
		comp = 1;
		goto end;
	    }
	}
	if (a_len != USUAL_BLOCKS) {
	    comp = 0;
	    goto end;
        }
    }
 end:

    fclose (a);
    fclose (b);
    
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

/* Reopen stdin to a new file.  Dies on failure. */

void 
freopen_stdin (const char * file_name)
{
    if (! freopen (file_name, "r", stdin)) {
	error ("could not open %s: %s", file_name, strerror (errno));
    }
}

/* Reopen stdout to a new file.  Dies on failure. */

void 
freopen_stdout (const char * file_name)
{
    if (! freopen (file_name, "w", stdout)) {
	error ("could not open %s: %s", file_name, strerror (errno));
    }
}

