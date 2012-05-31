/* Write prototypes for traditional C (no prototype) functions. */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "wt.h"
#include "error-msg.h"
#include "sys_or_exit.h"
#include "argument.h"
#include "traditional.h"
#include "c-word.h"

extern int yylineno;

/* Debugging flag for the functions of this file. */

int trad_debug;

/* Number of copies to request to `malloc', `realloc' etc. */

#define TRAD_INC 0x10

/* Arguments of traditional C function. */

static struct arg ** args;

/* Number of arguments currently seen. */

static int n_arg = -1;

/* Size of array `args'. */

static unsigned max_arg;

/* 
   Increment the total number of arguments, get more memory if
   necessary, and initialise the `in argument' state.
*/

static void
inc_n_arg (void)
{
    if (++n_arg >= max_arg) {
        unsigned i;
        max_arg += TRAD_INC;
        if (args) {
            args = realloc_or_exit (args, sizeof (struct arg *) * max_arg);
        }
        else {
            args = malloc_or_exit (sizeof (struct arg *) * max_arg);
        }
        for (i = n_arg; i < max_arg; i++) {
            args[i] = 0;
        }
    }
}

/* Deal with commas in argument lists. For example in

char a (b, c) char b, c; { ... }

the comma after the second `b' causes a call to this function. */

void
traditional_comma (void)
{
    arg_put_name (args[n_arg]);
    inc_n_arg ();
    args [n_arg] = arg_share (args[n_arg-1]);
    
#ifdef CFUNCTIONS_DEBUG
    if (trad_debug) {
        printf ( "traditional comma\n");
    }
#endif
}

/* Semicolon in argument list causes a call to this function. */

void
traditional_next (void)
{
#ifdef CFUNCTIONS_DEBUG
    if (trad_debug) {
        printf ( "traditional next\n");
    }
#endif
    arg_put_name (args[n_arg]);
    inc_n_arg ();
}

/* Save a C word which is either an argument type or an argument.  The
   context decides which one.  In the case of ambiguity it is
   temporarily made a type, and left to `traditional_next' to sort
   things out. */

void
traditional_save (char * text, unsigned length)
{
    if (n_arg == -1) {
        inc_n_arg ();
    }
    if (! args[n_arg]) {
        args[n_arg] = arg_start (0 /* no debug */);
    }
    arg_add (args[n_arg], text, yylineno);
}

/* Print the type of the variable `text' to `f'. */

void
traditional_print_type (FILE * f, char * text)
{
    int i;
    
#ifdef CFUNCTIONS_DEBUG
    if (trad_debug) {
        printf ("traditional print type `%s': \n", text);
    }
#endif

    for (i = 0; i < n_arg; i++) {
      /* Need to do this due to problem with `[]'. */

      unsigned len = c_word (args[i]->name->name);

#ifdef CFUNCTIONS_DEBUG
      if (trad_debug)
        printf ("comparing with %s\n", args[i]->name->name);
#endif

      if (strncmp ((char *) args[i]->name->name, text, len) == 0)
        {
          arg_fprint (f, args[i]);
          return;
        }
    }
    if (warns.implicit_int) {
        line_warning ("implicit int argument `%s'", text);
    }
    fprintf (f, "int /* default */ %s", text);
}

/* Free all the memory allocated and reset the counters. */

void
traditional_reset (void)
{
    int i;

#ifdef CFUNCTIONS_DEBUG
    if (trad_debug) {
        printf ("traditional reset\n");
    }
#endif

    for (i = 0; i <= n_arg; i++) {
        if (args[i]) {
            arg_free (args[i]);
        }
        args[i] = 0;
    }
    n_arg = -1;
}
