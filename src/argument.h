#ifndef CFH_ARGUMENT_H
#define CFH_ARGUMENT_H
struct type {
    unsigned char * name;
    struct type * next, * prev;
    unsigned line;
} 
;
struct shared_type {
    struct type * t;
    int ref_count;
} 
;
struct arg {
    enum {TYPE, POINTER, SUFFIX, CPP_MUDDLE} parse_state; 
    struct type * name;         
    struct shared_type * types; 
    struct type * pointers;     
    struct type * suffixes;     
    unsigned line;              
    struct arg * prev, * next;  
    char * function_pointer;
    char * function_pointer_arguments;
    unsigned is_typedef : 1;
    unsigned is_function_pointer : 1;
    unsigned debug : 1;
} 
;
struct arg * arg_start (int debug);
struct arg * arg_share (struct arg * a);
void arg_add (struct arg * a, const char * t, unsigned line);
void arg_put_name (struct arg * a);
void arg_free (struct arg * a);
void arg_fprint (FILE * f, struct arg * a);
void arg_fprint_all (FILE * f, struct arg * a, int do_extern);
void arg_tagable (struct arg * a);
#endif 
