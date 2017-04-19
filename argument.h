#ifndef CFH_ARGUMENT_H
#define CFH_ARGUMENT_H

/* This structure can store the information about the type of an
   argument, for example "int", "void", etc. It is also used to store
   the names of arguments. So in a declaration like "int a, b", three
   types, one each for "int", "a", and "b", will be created. */

struct type
{
    unsigned char * name;
    struct type * next, * prev;
    /* Line number of original file it was seen on. */
    unsigned line;
};

/* Some argument types are shared between several arguments.  For
   example in `int i, j;', the type `int' is shared between "i" and
   "j".  This structure stores those relationships. */

struct shared_type
{
    struct type * t;
    int ref_count;
};

/* A complete function argument. */

struct arg
{
    enum {TYPE, POINTER, SUFFIX, CPP_MUDDLE} parse_state; 
    /* For clarity. */
    struct type * name;         /* Name of this argument. */
    struct shared_type * types; /* Type. */
    struct type * pointers;     /* Pointer prefixes. */
    struct type * suffixes;     /* Array suffixes. */
    unsigned line;              /* Line number of the name. */
    struct arg * prev, * next;  /* For shared arguments. */
    
    /* Stopgaps. */
    char * function_pointer;
    char * function_pointer_arguments;
    /* This is true if the current argument is a typedef. */
    unsigned is_typedef : 1;
    /* This is true if the current argument is a function pointer. */
    unsigned is_function_pointer : 1;
    /* Print debugging messages. */
    unsigned debug : 1;
};

struct arg * arg_start ();
struct arg * arg_share (struct arg * a);
void arg_add (struct arg * a, const char * t, unsigned line);
void arg_put_name (struct arg * a);
void arg_free (struct arg * a);
void arg_fprint (FILE * f, struct arg * a);
void arg_fprint_all (FILE * f, struct arg * a, int do_extern);
void arg_tagable (struct arg * a);
void arg_memory_check ();
#endif 
