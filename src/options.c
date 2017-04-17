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

/* The following is compiled only for the standalone program.  It
   generates the file `options.texi' which describes command-line
   options and the Cfunctions manual page `cfunctions.1'.  */

#ifdef MANUAL

static const char * texi_begin = 
"@c This texinfo file is generated from `"__FILE__"'.\n"
"@noindent\n"
"Cfunctions can be configured with a file @file{.cfunctionsrc} in\n"
"the user's home directory.\n"
"The possible command-line and configuration options for Cfunctions are\n\n"
"@table @code\n";

static const char * texi_end = 
"@end table\n\n";

/* The header of the manual page. */

static const char * man_begin =
".\\\" This file is auto-generated from `"__FILE__"'.\n"
".SH OPTIONS\n";


static const char * man_end =
".\\\" End of file.\n";

/* Comparison routine for quicksort. */

static int
compare_letter_options (const void * a, const void * b)
{
    unsigned ia = * (unsigned *) a;
    unsigned ib = * (unsigned *) b;
    int av;
    int bv;
    av = tolower (long_options[ia].val);
    bv = tolower (long_options[ib].val);
    if (av == bv) {
        return strcmp (long_options[ia].name, long_options[ib].name);
    }
    return av - bv;
}

enum
{
    none,
    texi,
    man,
}
mode;

int
main (int argc, char *argv[])
{
    unsigned * order;
    unsigned i;

    program_name = "options";

    while (1) {
        i = getopt (argc, argv, "mtc");
        if (i == EOF) {
            break;
        }
        switch (i) {
        case 'm':
            mode = man;
            break;
        case 't':
            mode = texi;
            break;
        case '?':
            return 1;
        default:
            error ("unknown option: '%c'", i);
        }
    }
    if (optind < argc) {
        error ("can't have arguments");
    }
    if (mode == man) {
        fcopy (stdout, "man.head");
    }
    switch (mode) {
    case texi:
        printf ("%s", texi_begin);
        break;
    case man:
        printf ("%s", man_begin);
        break;
    default:
        error ("Unknown mode");
    }

    /* Sort things into the alphabetical order of the short options. */

    order = malloc_or_exit (sizeof (unsigned) * n_options);
    for (i = 0; i < n_options; i++) {
        order [i] = i;
    }
    qsort (order, n_options, sizeof (* (order)), compare_letter_options);
    
    for (i = 0; i < n_options; i++) {
        unsigned j = order[i];
        if (! usage[j]) {
            error ("empty usage message for option '%c'", long_options[j].val);
        }
        switch (mode) {
        case texi:
            printf ("@cindex @code{-%c}\n@cindex @code{--%s}\n",
                    long_options [j].val, long_options [j].name);
            printf ("@item -%c ",             long_options [j].val);
            if (long_options [j].has_arg == required_argument) {
                printf (" @var{argument}");
            }
            printf ("\n@itemx --%s ", long_options [j].name);
            if (long_options [j].has_arg == required_argument) {
                printf (" @var{argument}");
            }
            printf ("\n%s.\n", usage[j]);
/*
            if (other_stuff[j].cross_ref) {
                printf ("@xref{%s}.\n", other_stuff[j].cross_ref);
            }
            if (other_stuff[j].rc_name) {
                printf ("This can also be controlled with a line `%s' "
                        "in @file{.cfunctionsrc}\n", 
                        other_stuff[j].rc_name);
            }
            printf ("\n");
*/
            break;
        case man:
            printf (".TP\n\\fB-%c\\fP", long_options [j].val);
            if (long_options [j].has_arg == required_argument) {
                printf (" \\fIargument\\fP");
            }
            printf (", \\fB--%s\\fP", long_options [j].name);
            if (long_options [j].has_arg == required_argument) {
                printf (" \\fIargument\\fP");
            }
            printf ("\n%s.\n", usage[j]);
/*
            if (other_stuff[j].cross_ref) {
                printf ("See node `%s' in the info documentation.\n", 
                        other_stuff[j].cross_ref);
            }
            if (other_stuff[j].rc_name) {
                printf ("This can also be controlled with a line `%s' "
                        "in $HOME/.cfunctionsrc\n", 
                        other_stuff[j].rc_name);
            }
*/
            break;
        default:
            error ("Unknown mode");
        }
    }
    switch (mode) {
    case texi:
        printf ("%s", texi_end);
        break;
    case man:
        printf ("%s", man_end);
        fcopy (stdout, "man.tail");
        break;
    default:
        error ("Unknown mode");
    }
    return 0;
}

#endif /* MANUAL */
