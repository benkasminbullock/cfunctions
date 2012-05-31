/* Command line options for Cfunctions.  Also writes documentation. */

#ifdef HEADER
#include <getopt.h>
#endif /* HEADER */
#ifdef MANUAL
#include <ctype.h>
#include <stdlib.h>
#endif /* MANUAL */

#include "sys_or_exit.h"
#include "error_msg.h"
#include "options.h"
#include "config.h"
#include "file.h"

struct option long_options [] = {
    {"advert",          required_argument, NULL, 'a'},
    {"backup",          no_argument,       NULL, 'b'},
    {"copy-c-ex",       no_argument,       NULL, 'c'},
    {"cpp",             no_argument,       NULL, 'C'},
#ifdef CFUNCTIONS_DEBUG                       
    {"debug",           required_argument, NULL, 'D'},
#endif                                            
    {"etags",           no_argument,       NULL, 'e'},
    {"extensions",      no_argument,       NULL, 'x'},
    {"file",            required_argument, NULL, 'f'},
    {"global",          required_argument, NULL, 'g'},
    {"global-macro",    required_argument, NULL, 'G'},
    {"individual",      no_argument,       NULL, 'i'},
    {"include-c-ex",    no_argument,       NULL, 'I'},
    {"help",            no_argument,       NULL, 'h'},
    {"keep",            no_argument,       NULL, 'k'},
    {"line-numbers",    no_argument,       NULL, 'n'},
    {"local",           required_argument, NULL, 'l'},
    {"local-macro",     required_argument, NULL, 'L'},
    {"output",          required_argument, NULL, 'o'},
    {"proto-macro",     required_argument, NULL, 'p'},
    {"cpp-arg",         required_argument, NULL, 'P'},
    {"static",          no_argument,       NULL, 's'},
    {"suffix",          required_argument, NULL, 'S'},
    {"tags",            no_argument,       NULL, 't'},
    {"no-upgrade",      no_argument,       NULL, 'u'},
    {"version",         no_argument,       NULL, 'v'},
    {"version-control", required_argument, NULL, 'V'},
    {"wrap",            required_argument, NULL, 'w'},
    {"write-comments",  no_argument,       NULL, 'm'},
    {"warn",            required_argument, NULL, 'W'},
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
    "If "arg" is `off', don't print an advert.  If "arg" is a file name, "
    "print the file instead",
    "Back up `.h' files",
    "Copy the `c-extensions.h' into the header file",
    "Pass input through the C preprocessor",
#ifdef CFUNCTIONS_DEBUG
    "Set debugging option "arg" (-D help for list)",
#endif
    "Create an Emacs tag table",
    "Don't copy the `c-extensions.h' file",
    "Use "arg" as the file name in #line directives (with -n option)",
    "Write one global header for all C files",
    "Give global header macro the name "arg,
    "Write an individual header for each C file",
    "Write #include <c-extensions.h>",
    "Print a help message and exit",
    "Don't delete generated .h files which are empty",
    "Write #line information in the .h files",
    "Write one local header file for all C files",
    "Give local header macro the name "arg,
    "Redirect standard output to "arg,
    "Give prototype macro the name "arg,
    "Pass "arg" to the C preprocessor",
    "Don't ignore things declared `static'",
    "Use suffix `"arg"' for simple backups",
    "Create a tag table",
    "Don't upgrade traditional C",
    "Write version information and exit",
    "Use version control style "arg,
    "Write a wrapper when writing to stdout",
    "Write comments to header file",
    "Warn about malpractices"
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

/* Most of the options are cross-referenced to somewhere else in the
   manual.  The following strings point to a node in the Cfunctions
   manual.  */

static struct
{
    const char * cross_ref;
    const char * rc_name;
}
other_stuff [ N_OPTIONS ] =
{
  {"Advertisement", "advert"}, 
  {"Backups", "backup"},
  {"C extensions", "copy c ex"},
  {"CPP"},
#ifdef CFUNCTIONS_DEBUG
  {NULL},
#endif
  {NULL},
  {"Input file format"},
  {"One header"},
  {"Extra header information", "global macro"},
  {"Individual headers"},
  {"C extensions", "include c ex"},
  {"C compiler", "line numbers"},
  {NULL},
  {NULL},
  {"C compiler", "line numbers"},
  {"Extra local header information"},
  {"Extra local header information", "local macro"},
  {"Output files"},
  {"Traditional C", "proto macro"},
  {"CPP"},
  {"Ignoring static"},
  {"Backups", "backup suffix"},
  {"Making tag tables"},
  {NULL},
  {"Backups", "version control"},
  {"Wrappers"},
  {"Comments", "write comments"},
  {"Warnings", "warn"}
};

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
    int av, bv;
    av = tolower (long_options[ia].val);
    bv = tolower (long_options[ib].val);
    if (av == bv) {
        return strcmp (long_options[ia].name, long_options[ib].name);
    }
    return av - bv;
}

enum {
    none,
    texi,
    man,
} mode;

int
main(int argc, char *argv[])
{
    unsigned * order;

    program_name = "options";

    while (1) {
        unsigned i;
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
    if (optind < argc) 
        error ("can't have arguments");

    if (mode == man)
        fcopy (stdout, "man.head");

    switch (mode) {
    case texi:
        printf ("%s", texi_begin);
        break;
    case man:
        printf ("%s", man_begin);
        break;
    case cgi:
    default:
        error ("");
    }

    /* Sort things into the alphabetical order of the short options. */

    order = malloc_or_exit (sizeof (unsigned) * n_options);
    for (i = 0; i < n_options; i++) {
        order [i] = i;
    }
    qsort (order, n_options, sizeof (unsigned), compare_letter_options);
    
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
            if (other_stuff[j].cross_ref) {
                printf ("@xref{%s}.\n", other_stuff[j].cross_ref);
            }
            if (other_stuff[j].rc_name) {
                printf ("This can also be controlled with a line `%s' "
                        "in @file{.cfunctionsrc}\n", 
                        other_stuff[j].rc_name);
            }
            printf ("\n");
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
            if (other_stuff[j].cross_ref) {
                printf ("See node `%s' in the info documentation.\n", 
                        other_stuff[j].cross_ref);
            }
            if (other_stuff[j].rc_name) {
                printf ("This can also be controlled with a line `%s' "
                        "in $HOME/.cfunctionsrc\n", 
                        other_stuff[j].rc_name);
            }
            break;
        default:
            error ("");
        }
    }
    switch (mode) {
    case texi:
        printf (texi_end);
        break;
    case man:
        printf (man_end);
        fcopy (stdout, "man.tail");
        break;
    default:
        error ("");
    }
    return 0;
}

#endif /* MANUAL */
