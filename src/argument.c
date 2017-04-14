/* Functions to help parse a C declaration. */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>

#include "wt.h"
#include "sys_or_exit.h"
#include "error-msg.h"
#include "argument.h"

/* Macro for printing debugging statements. */

#define DBMSG(format,msg...) do {                               \
        printf ("%s:%d [%s]: ", __FILE__, __LINE__, __func__);	\
        printf (format, ## msg);                                \
    } while (0)

/* Create a completely new "arg" structure. */

struct arg * 
arg_start (int debug)
{
    struct arg * a;

    a = calloc_or_exit (1, sizeof (struct arg));
    a->types = calloc_or_exit (1, sizeof (struct shared_type));
    a->types->ref_count++;
    a->debug = debug;
    return a;
}

/* Create a new "arg" with the same type as "a". */

struct arg * 
arg_share (struct arg * a)
{
    struct arg * b;

    b = calloc_or_exit (1, sizeof (struct arg));
    b->types = a->types;
    b->types->ref_count++;
    b->parse_state = POINTER;
    b->prev = a;
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

extern unsigned yylineno;

static void
check_cpp_muddle (struct arg * a, const char * t, unsigned line)
{
    char * at;

    at = strchr (t, '@');
    if (at) {
	a->parse_state = CPP_MUDDLE;
	line_warning ("cfunctions was unable to adequately cope with "
		      "some complex C preprocessor instructions");
    }
}

/* Add another string "t" to an argument "a" at line "line". */

void
arg_add (struct arg * a, const char * t, unsigned line)
{
    unsigned t_len;
    struct type * x;


    if (a->debug) {
        DBMSG ("Adding '%s' in state %d\n", t, a->parse_state);
    }



    t_len = strlen (t);
    x = calloc_or_exit (1, sizeof (struct type));
    x->name = malloc_or_exit (t_len + 1);
    strcpy ((char *) x->name, t);
    x->line = line;
    if (t[0] == '*') {
        a->parse_state = POINTER;
    }
    if (a->parse_state == SUFFIX) {
        check_cpp_muddle (a, t, line);
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

            if (a->debug) {
                DBMSG ("Adding '%s' to list of types\n", t);
            }

            type_list_add (& a->types->t, x);
        }
        break;

    case CPP_MUDDLE:
        /* A C preprocessor statement came after the closing bracket of
           an argument. */
        break;

    default:
        bug (HERE, "bad value in switch");
    }
}

/* Move a word out of a type list and into the name of an argument. */

static void
type_list_move (struct arg * a, struct type ** t)
{
    a->name = * t;
    if ((* t)->prev) {
	(* t)->prev->next = NULL;
    }
    * t = (* t)->prev;
}

/* For a particular argument, make the last-seen word the name
   of the argument. */

void
arg_put_name (struct arg * a)
{

    if (a->debug) {
        DBMSG ("putting name\n");
    }

    /* Kludge. */
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
#if 0
    while ((t = t->next)) {
	free (t->prev);
    }
#else
    while (t->next) {
#if 0
	printf ("Freeing %s\n", t->name);
#endif
	free (t->name);
	t = t->next;
	free (t->prev);
    }
#if 0
    printf ("Freeing %s\n", t->name);
#endif
    free (t->name);
    free (t);
#endif
}

/* Free the memory associated with an argument. */

void
arg_free (struct arg * a)
{
    a->types->ref_count--;
    if (a->types->ref_count == 0) {
#if 0
	printf ("zero refs\n");
#endif
	type_free (a->types->t);
	free (a->types);
    }
    if (a->name) {
#if 0
	printf ("Freeing `%s'\n", a->name->name);
#endif
	free (a->name->name);
	free (a->name);
    }
    type_free (a->pointers);
    type_free (a->suffixes);
    if (a->next) {
	a->next->prev = NULL;
    }
    if (a->prev) {
	arg_free (a->prev);
    }
    free (a);
}

extern void cpp_eject (unsigned);

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
    do {
	if (t->name[0] == '@') {
	    unsigned cpp_id;
	    if (strncmp ((char *) t->name + 1, "CPP", 3) == 0) {
		cpp_id = atoi ((char *)t->name + 4);

		/* I removed the `extern' printing bit. */

		cpp_eject (cpp_id);
	    }
	    else
		bug (HERE, "unknown @ escape `%s'", t->name);
	}
	else {
	    if (do_extern && ! did_extern) {
		fprintf (f, "extern ");
		did_extern = 1;
	    }
	    fprintf (f, "%s ", t->name);
	}
	t = t->next;
    }
    while (t);
}

/* Print just one argument. */

void
arg_fprint (FILE * f, struct arg * a)
{

    if (a->debug) {
        DBMSG ("Printing one argument.\n");
    }


    if (a->types->t) {
        type_fprint (f, a->types->t, 0);
    }
    else {
        if (warns.implicit_int) {
            line_warning ("function `%s' is implicit int", a->name->name);
        }
        fprintf (outfile, "int /* default */");
    }
    type_fprint (f, a->pointers, 0);
    if (a->name) {
        fprintf (f, "%s", a->name->name); 
        suffix_fprint (f, a);
    }
}

/* Print all the arguments in a list of them.  This is used for
   printing a list of globals, so the `extern' prefix is always used
   here. */

void
arg_fprint_all (FILE * f, struct arg * a, int do_extern)
{

    if (a->debug) {
        DBMSG ("Printing all arguments: do extern is %d.\n", do_extern);
        if (a->is_function_pointer) {
            DBMSG ("Function pointer.\n");
        }
    }


    if (a->types->t) {

        if (a->debug) {
            DBMSG ("doing type_fprint.\n");
        }

        type_fprint (f, a->types->t, do_extern);
    }

    else {
        if (a->debug) {
            DBMSG ("No types.\n");
        }
    }

    /* Kludge. */
    if (a->is_function_pointer) {
        fprintf (f, "(* %s) (%s",
                 a->function_pointer,
                 a->function_pointer_arguments);
    }
    else {
	while (a->prev) {
	    a = a->prev;
	}
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
}

/* If there is anything tagable in the type field of `a' then make it
   a tag.  This is for struct, union and enum name tagging only. */

void
arg_tagable (struct arg * a)
{
    struct type * t = a->types->t;
    if (! t) {
	return;
    }
    if (strcmp ((char *) t->name, "union") == 0) {
	return;
    }
    if (strcmp ((char *) t->name, "struct") == 0) {
	return;
    }
    if (strcmp ((char *) t->name, "enum") == 0) {
	return;
    }
    if (strcmp ((char *) t->name, "typedef") == 0) {
	return;
    }
}
