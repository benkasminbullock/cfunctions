/* Functions to help parse a C declaration. */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "wt.h"
#include "sys-or-exit.h"
#include "error-msg.h"
#include "argument.h"

/* This variable tracks the number of times that functions which
   allocate and free memory are called. */

static int arg_n_mallocs;

/* This function creates and returns a new "struct arg" structure with
   zero values. */

static struct arg *
new_arg ()
{
    struct arg * value;
    value = calloc_or_exit (1, sizeof (* value));
    arg_n_mallocs++;
    return value;
}

/* This function creates a new "arg" structure with an allocated
   "types" field. */

struct arg * 
arg_start ()
{
    struct arg * a;

    a = new_arg ();
    a->types = calloc_or_exit (1, sizeof (* a->types));
    arg_n_mallocs++;
    a->types->ref_count = 1;
    return a;
}

/* Create a new "arg" with the same type as "a". */

struct arg * 
arg_share (struct arg * a)
{
    struct arg * b;

    b = new_arg ();
    b->types = a->types;
    if (a->types->ref_count == 0) {
	bug (HERE, "zero ref count in copied argument");
    }
    b->types->ref_count++;
    b->parse_state = POINTER;
    b->prev = a;
    if (a->next) {
	bug (HERE, "non-zero next argument in arg");
    }
    a->next = b;
    return b;
}

/* Add type "b" to a linked list of types "a". */

static void
type_list_add (struct type ** a, struct type * b)
{
    if (* a) {
        (* a)->next = b;
        b->prev = * a;
    }
    * a = b;
}

/* Check for a muddled situation */

static void
check_cpp_muddle (struct arg * a, const char * t)
{
    char * at;

    at = strchr (t, '@');
    if (at) {
	a->parse_state = CPP_MUDDLE;
	line_warning ("cfunctions was unable to adequately cope with "
		      "some complex C preprocessor instructions");
    }
}

/* Add another string "t" to an argument "a" at line "line". This is
   called from cfunctions.fl upon seeing [] at the end of a C
   declaration. */

void
arg_add (struct arg * a, const char * t)
{
    unsigned t_len;
    struct type * x;
    t_len = strlen (t);
    if (t_len == 0) {
	bug (HERE, "Length of string is zero in arg_add");
    }
    x = calloc_or_exit (1, sizeof (struct type));
    arg_n_mallocs++;
    x->name = malloc_or_exit (t_len + 1);
    arg_n_mallocs++;
    memcpy (x->name, t, t_len + 1);
    if (t[0] == '*') {
        a->parse_state = POINTER;
    }
    if (a->parse_state == SUFFIX) {
        check_cpp_muddle (a, t);
    }
    switch (a->parse_state) {
    case SUFFIX:
        if (! strchr (t, '[')) {
            bug (HERE, "no [ in suffix, string is %s", t);
        }
        type_list_add (& a->suffixes, x);
        break;

    case POINTER:
        type_list_add (& a->pointers, x);
        break;
      
    case TYPE:
        if (a->types->ref_count > 1) {
            bug (HERE, "addition to a shared type list");
        }
        else {
            type_list_add (& a->types->t, x);
        }
        break;

    case CPP_MUDDLE:
        /* A C preprocessor statement came after the closing bracket of
           an argument. */
        break;

    default:
        bug (HERE, "a->parse_state had bad value %d", a->parse_state);
    }
}

/* Move a word out of a type list and into the name of an argument. */

static void
type_list_move (struct arg * a, struct type ** t_ptr)
{
    struct type * t;
    t = * t_ptr;
    a->name = t;
    if (t->prev) {
	t->prev->next = NULL;
    }
    * t_ptr = t->prev;
}

/* For a particular argument, make the last-seen word the name
   of the argument. */

void
arg_put_name (struct arg * a)
{
    if (a->is_function_pointer) {
        return;
    }
    if (a->name) {
        return;
    }
    switch (a->parse_state) {
    case POINTER:
	if (! a->pointers) {
	    bug (HERE, "attempt to move non-existent name from pointer list");
	}
	type_list_move (a, & a->pointers);
	break;

    case TYPE:
	if (! a->types->t) {
	    return;
	}
	if (a->types->ref_count > 1) {
	    bug (HERE, "attempt to move a shared type to arg name");
	}
	type_list_move (a, & a->types->t);
	break;

    case CPP_MUDDLE:
	break;

    case SUFFIX:
	bug (HERE, "attempt to move a suffix to arg name");

    default:
        bug (HERE, "a->parse_state had bad value %d", a->parse_state);
    }

    if (! a->name) {
	bug (HERE, "Could not find a name");
    }

    /* Anything which comes now must be a suffix. */

    a->parse_state = SUFFIX;
}

/* Free a list of types.  Things are arranged so that the string which
   is part of the type is always at the end of the type, so it is not
   necessary to free that string separately. */

static void
type_free (struct type * t)
{
    if (! t) {
	return;
    }
    while (t->prev) {
	t = t->prev;
    }
    while (t) {
	struct type * prev;
	prev = t;
	t = prev->next;
	free_or_exit (prev->name);
	prev->name = 0;
	arg_n_mallocs--;
	free_or_exit (prev);
	prev = 0;
	arg_n_mallocs--;
    }
}

/* Free the memory associated with an argument. */

void
arg_free (struct arg * a)
{
    a->types->ref_count--;
    if (a->types->ref_count == 0) {
	type_free (a->types->t);
	free_or_exit (a->types);
	a->types = 0;
	arg_n_mallocs--;
    }
    if (a->name) {
	free_or_exit (a->name->name);
	a->name->name = 0;
	arg_n_mallocs--;
	free_or_exit (a->name);
	a->name = 0;
	arg_n_mallocs--;
    }
    type_free (a->pointers);
    type_free (a->suffixes);
    if (a->prev) {
	arg_free (a->prev);
    }
    if (a->is_function_pointer) {
	if (a->function_pointer) {
	    free_or_exit (a->function_pointer);
	    a->function_pointer = 0;
	}
    }
    if (a->function_pointer_arguments) {
	free_or_exit (a->function_pointer_arguments);
	a->function_pointer_arguments = 0;
    }
    free_or_exit (a);
    a = 0;
    arg_n_mallocs--;
}


/* Print a list of array suffixes to the specified stream. */

static void
suffix_fprint (FILE * f, struct arg * a)
{
    struct type * t;
    t = a->suffixes;
    if (! t) {
	return;
    }
    while (t->prev) {
	t = t->prev;
    }
    if (a->is_typedef) {
	fprintf (f, "%s", t->name);
    }
    else {
	fprintf (f, "[]");
    }
    t = t->next;
    while (t) {
	fprintf (f, "%s", t->name);
	t = t->next;
    }
}


/* Print a list of types to the specified stream. */

static void
type_fprint (FILE * f, struct type * t, int do_extern)
{
    int did_extern = 0;
    if (! t) {
	return;
    }
    while (t->prev) {
	t = t->prev;
    }
    while (t) {
	if (t->name[0] == '@') {
	    unsigned cpp_id;
	    if (strncmp ((char *) t->name + 1, "CPP", 3) == 0) {
		cpp_id = atoi ((char *)t->name + 4);
		cpp_eject (cfp, cpp_id);
	    }
	    else {
		bug (HERE, "unknown @ escape `%s'", t->name);
	    }
	}
	else {
	    if (do_extern && ! did_extern) {
		fprintf (f, "extern ");
		did_extern = 1;
	    }
	    fprintf (f, "%s", t->name);
	}
	t = t->next;
	if (t) {
	    fprintf (f, " ");
	}
    }
}

/* Print just one argument. */

void
arg_fprint (FILE * f, struct arg * a)
{
    if (a->types->t) {
        type_fprint (f, a->types->t, 0);
    }
    else {
	line_warning ("function `%s' is implicit int", a->name->name);
        fprintf (f, "int /* default */");
    }
    type_fprint (f, a->pointers, 0);
    if (a->name) {
        fprintf (f, " %s", a->name->name); 
        suffix_fprint (f, a);
    }
}

/* Print all the arguments in "a" to "f". "do_extern" controls whether
   to print the "extern" keyword, and it is set to a false value only
   if copying a variable declaration verbatim into the header file,
   which is generally a bad idea. */

void
arg_fprint_all (FILE * f, struct arg * a, int do_extern)
{
    if (a->types->t) {
        type_fprint (f, a->types->t, do_extern);
    }

    if (a->is_function_pointer) {
        fprintf (f, "(* %s) (%s",
                 a->function_pointer,
                 a->function_pointer_arguments);
	return;
    }

    while (a->prev) {
	a = a->prev;
    }
    fprintf (f, " ");
    for (; a; a = a->next) {
	type_fprint (f, a->pointers, 0);
	if (a->name) {
	    fprintf (f, "%s", a->name->name);
	}
	suffix_fprint (f, a);
	if (a->next) {
	    fprintf (f, ", ");
	}
    }
}

/* Check that all the memory allocated within this file has been
   freed. */

void
arg_memory_check ()
{
    if (arg_n_mallocs != 0) {
	fprintf (stderr, "%s:%d: arg_n_mallocs = %d\n",
		 __FILE__, __LINE__, arg_n_mallocs);
    }
}
