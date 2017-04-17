/* This file is for printing a usage message about a program to stderr
   or stdout, with formatting of the descriptive parts and long
   options. */

#include <stdio.h>
#include <getopt.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>

#include "error-msg.h"
#include "sys_or_exit.h"
#include "print-usage.h"

/* The maximum number of characters printable on a line. This is set
   in wt.c so it has to be a global variable. */

unsigned error_msg_width = 78;

/* Print a string which does not go past the end of the terminal
   size. */

static void
print_offset (FILE * f, const char * orig, int orig_len, unsigned indent,
              unsigned n_chars)
{
    char * msg = malloc_or_exit (orig_len);
    unsigned printed;
  
    while (orig_len > 0) {
        char * space;
        if (orig_len > error_msg_width - indent) {
            strncpy (msg, orig, error_msg_width - indent);
            msg[error_msg_width - indent] = '\0';
            space = strrchr (msg, ' ');
            if (space) {
                space++;
                * space = '\0';
            }
        }
        else {
            strcpy (msg, orig);
        }
        printed = strlen (msg);
        orig += printed;
      
        if (n_chars < indent) {
            fprintf (f, "%*s", indent - n_chars, " ");
        }
        else {
            fprintf (f, "\n%*s\n", indent, " ");
        }
        fprintf (f, "%s\n", msg);
        n_chars = 0;
        orig_len -= printed;
    }
    free (msg);
}


/* I'm not sure what this is but it should be a macro. */

#define THIRTY 30

/* Print a usage message on stderr describing the options
   available. The arrays `long_options' and `usage' must have the same
   number of members, n_options. 

   The struct `struct option' is defined in `getopt.h' which is part
   of the GNU C library.  */

void 
print_usage (unsigned n_options, struct option * long_options, 
             const char ** usage)
{
    unsigned i, n_chars;

    fprintf (stderr, "%s: options are:\n", program_name );

    for (i = 0; i < n_options; i++) {
        int l;

        n_chars = fprintf ( stderr, "-%c (--%s)", long_options[ i ].val, 
                            long_options[ i ].name );

        switch (long_options[ i ].has_arg) {
        case required_argument:
            n_chars += fprintf (stderr, " <arg> ");
            break;
        case optional_argument:
            n_chars += fprintf (stderr, " [arg] ");
            break;
        case no_argument:
            break;
        default:
            bug ( HERE, "strange has_arg field %d in long_options %d", 
                  long_options[i].has_arg, i ); 
        }

        l = strlen (usage[i]);

        if (l < error_msg_width - THIRTY) {
            if (n_chars < THIRTY) {
                fprintf (stderr, "%*s%s\n", THIRTY - n_chars, " ", usage[i]);
            }
            else {
                fprintf (stderr, "\n%*s%s\n", THIRTY, " ", usage[i]);
            }
        }
        else {
            print_offset (stderr, usage[i], l, THIRTY, n_chars);
        }
    }
}

