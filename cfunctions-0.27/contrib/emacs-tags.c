#include "file.h"

/* File containing the generated tags table. */

FILE * cf_tags_file;

/* The name of the source file. */

static char * source_name;

/* The line that Emacs-tags is currently looking at of the source
   file. */

static unsigned line;

/*
  Read `n_lines' lines of file `f'.
*/

static void
fread_lines ( FILE * f, unsigned n_lines )
{
  unsigned i;

  for (i = 0;
}

int main ()
{
  fopen_or_exit (cf_tags_file, "CFTAGS");

}
