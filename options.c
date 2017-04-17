/* Command line options for Cfunctions.  Also writes documentation. */

#include <getopt.h>
#include <stdio.h>
#include <sys/stat.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include "sys_or_exit.h"
#include "error-msg.h"
#include "options.h"
#include "file.h"

struct option long_options [] = {
    {"debug",           required_argument, NULL, 'D'},
    {0, 0, 0, 0}
};

#define N_OPTIONS sizeof (long_options) / sizeof (struct option) - 1

int n_options = N_OPTIONS;

#ifdef MANUAL 
#define arg "@var{argument}"
#else
#define arg "<arg>"
#endif /* MANUAL */

const char * usage [ N_OPTIONS ] = {
    "Set debugging option "arg" (-D help for list)",
};

/* 
   Extract a character string from a `struct option' (from GNU getopt)
   suitable for use with `getopt_long'.  

   Return value: allocated memory containing an option string.
*/

char * 
short_options (struct option * long_options, unsigned n_options)
{
    char * opt;
    char * optn;
    unsigned i;

    opt = malloc_or_exit (n_options * 2);

    /* Write point of the string. */

    optn = opt;

    for (i = 0; i < n_options; i++) {
        switch (long_options[i].has_arg) {
        case required_argument:
            optn += sprintf (optn, "%c:", long_options[i].val);
            break;
        case no_argument:
            optn += sprintf (optn, "%c", long_options[i].val);
            break;
        case optional_argument:
            optn += sprintf (optn, "%c::", long_options[i].val);
            break;
        default:
            error ("unknown `has_arg' field in long_options with name %s",
                   long_options[i].name);
        }
    }
    return opt;
}

