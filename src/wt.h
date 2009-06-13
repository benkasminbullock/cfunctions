/* This is a Cfunctions (version 0.25) generated header file.
   Cfunctions is a free program for extracting headers from C files.
   Get Cfunctions from `url is not available now'. */

/* This file was generated with:
`cfunctions -inmb wt.c' */
#ifndef CFH_WT_H
#define CFH_WT_H

/* From `wt.c': */

#line 51 "wt.c"
#undef BOOL
#define BOOL int
enum bool { FALSE, TRUE }
#line 54 "wt.c"
;

#line 85 "wt.c"
struct {
  unsigned index;
  unsigned value[2];
} 
/* Temporarily, this is a `tentative definition'.  The fix I will make
   is to make PRINT_FORMAT just one rule. */

#line 93 "wt.c"
pf;
/* Files to write output to.  The file `outfile' is the usual output
   file.  The file `localfile' is the file for outputting things which
   are prefixed with `LOCAL'.  The file `verbatimfile' is the file to
   write verbatim output to.  This can be either `localfile' or
   `outfile'. */

#line 102 "wt.c"
extern FILE * outfile;

#line 103 "wt.c"
extern FILE * localfile;

#line 104 "wt.c"
extern FILE * verbatim_file;

#line 109 "wt.c"
extern char line_source_name[];
/* Command line option-related variables.  Each variable is followed
   by the short form of the option which affects it.  See the arrays
   `usage' and `long_options' in `options.c' for explanation. */

#line 123 "wt.c"
extern const char * advert;
/* -a */

#line 124 "wt.c"
extern BOOL advert_arg;
/* -a */

#line 125 "wt.c"
extern BOOL backup;
/* -b */

#line 126 "wt.c"
extern BOOL copy_c_ex;
/* -c */

#line 127 "wt.c"
extern BOOL c_preprocess;
/* -C */

#line 128 "wt.c"
extern BOOL individual;
/* -i */

#line 129 "wt.c"
extern BOOL c_ex_std_include;
/* -I */

#line 130 "wt.c"
extern BOOL keep_empty_files;
/* -k */

#line 131 "wt.c"
extern BOOL write_comments;
/* -m */

#line 132 "wt.c"
extern BOOL write_line_numbers;
/* -n */

#line 133 "wt.c"
extern BOOL save_static_funcs;
/* -s */

#line 134 "wt.c"
extern char * wrap;
/* -w */

#line 135 "wt.c"
extern BOOL extensions;
/* -x */

#line 137 "wt.c"
extern const char * local_macro;
/* -L */

#line 138 "wt.c"
extern const char * global_macro;
/* -l */

#line 154 "wt.c"
extern const char * prototype_macro;
/* The wrapper for inline macros for GNU C. */

#line 158 "wt.c"
extern const char * inline_macro;
/* The depth of braces `{' and `}' seen. */

#line 162 "wt.c"
extern unsigned curly_braces_depth;

#line 176 "wt.c"
typedef enum {
  CPP_ZAP, 
  CPP_IF, CPP_ELSE, CPP_ELIF, CPP_ENDIF, N_CPP_IF_TYPES
}/* Different kinds of preprocessor conditional statements */

#line 185 "wt.c"
Cpp_If_Type;
/* This keeps track of how many lines have been printed by the CPP
   output part. */

#line 219 "wt.c"

#ifdef CFUNCTIONS_DEBUG
extern unsigned cpp_prints;
/* The top of the `cpp_if_stack' stack. */

#line 225 "wt.c"

#endif /* CFUNCTIONS_DEBUG */
extern unsigned cpp_if_now;
/* The top-level thing which is being parsed. */

#line 231 "wt.c"
extern struct arg * current_arg;
/* The number of brackets inside an argument list.  Brackets inside an
   argument list are most likely to happen for function pointer
   arguments.

   Deficiency: I am not sure where else it could happen.  There is a
   MAX variable just in case things go wrong. */

#line 240 "wt.c"
extern unsigned arg_br_depth;
/* The number of prototypes written to the local header file.  If this
   is zero, then Cfunctions knows that the local header file has no
   content and can be removed. */

#line 246 "wt.c"
extern unsigned n_local_writes;
/* Are we writing an inline function?  This is not part of the parsing
   state because it needs to survive through `function_reset'. */

#line 273 "wt.c"
extern BOOL inlining;
/* Is Cfunctions copying everything?  (this is set by `#ifdef HEADER'
   statements) */

#line 278 "wt.c"
extern BOOL verbatiming;
/* Is Cfunctions copying a `.hin' file into the header file? */

#line 282 "wt.c"
extern BOOL hin_copying;
/* Are we reading from standard input? */

#line 364 "wt.c"
extern BOOL reading_from_stdin;
/* Are we writing to standard output? */

#line 368 "wt.c"
extern BOOL writing_to_stdout;
/* Line number of rule matched. */

#line 372 "wt.c"
extern unsigned rule_line;

#line 376 "wt.c"
struct warning {
  unsigned implicit_int      : 1;
  unsigned strict_prototypes : 1;
  unsigned reserved_words    : 1;
} 
/* Warnings. */

#line 382 "wt.c"
;

#line 385 "wt.c"
extern struct warning warns;
/* count a `{'. */

#line 433 "wt.c"
void brace_open (void);
/* count a `}'. */

#line 448 "wt.c"
void brace_close (void);

#line 497 "wt.c"
void do_comment_start (void);

#line 512 "wt.c"
void do_comment_end (void);

#line 526 "wt.c"
void do_comment_print (const char * text , int leng );
/* The name do_PRINT_FORMAT is misleading: this is a temporary fix. */

#line 559 "wt.c"
void do_PRINT_FORMAT (void);

#line 566 "wt.c"
void do_void_pointer (const char * text );

#line 577 "wt.c"
void do_start_arguments (void);

#line 585 "wt.c"
void do_arguments (void);

#line 592 "wt.c"
void do_function_pointer (const char * text );

#line 603 "wt.c"
void do_word (const char * text , int leng );

#line 612 "wt.c"
void do_typedef (const char * text , int leng );

#line 621 "wt.c"
void inline_print (const char * x );
/* Deal with preprocessor `#line' directives. */

#line 632 "wt.c"
void line_change (const char * text );
/* Cfunctions sometimes needs to include `c-extensions.h' because it
   defines all the information about C extensions. */

#line 708 "wt.c"
void copy_c_extensions (void);
/* Check whether the user has asked for the C extensions, which are
   turned off by default.  If the user needs them, then write a
   statement #include "c-extensions.h".  The user needs the extensions
   file also if he uses traditional C declarations, because the
   definition of a macro `PROTO' for the traditional C declarations is
   in there.  */

#line 777 "wt.c"
void check_extensions (void);
/* Push a new thing onto the preprocessor conditional stack. */

#line 1057 "wt.c"
void cpp_add (char * text , Cpp_If_Type type );
/* Write out the preprocessor statement on the stack at position `u'.  */

#line 1204 "wt.c"
void cpp_eject (unsigned u );

#line 1286 "wt.c"
void do_start_cpp (const char * text );

#line 1295 "wt.c"
void do_escaped_brace (const char * text );

#line 1306 "wt.c"
void do_extern (const char * text , int leng );

#line 1315 "wt.c"
void do_NO_RETURN (const char * text );

#line 1325 "wt.c"
void do_arguments_close_bracket (const char * text , int leng );

#line 1346 "wt.c"
void do_LOCAL (const char * text );

#line 1354 "wt.c"
void do_INLINE (const char * text );

#line 1364 "wt.c"
void do_static (const char * text , int leng );

#line 1371 "wt.c"
void do_void (const char * text , int leng );

#line 1378 "wt.c"
void do_NO_SIDE_FX (const char * text );

#line 1386 "wt.c"
void do_arguments_open_bracket (const char * text , int leng );

#line 1394 "wt.c"
void do_brace_close (void);
/* Handle `(void)' argument declarations. */

#line 1429 "wt.c"
void do_void_arguments (void);

#line 1437 "wt.c"
void do_define (const char * text );
/* Nullify elements of the stack and release their memory. */

#line 1453 "wt.c"
void cpp_stack_free (unsigned p );
/* Zap pairs of #if and #endif where #if is `external'. */

#line 1463 "wt.c"
void cpp_external_tidy (void);
/* Print out any CPP things which have not yet been printed but which
   are not part of a complete #if and #endif block. */

#line 1512 "wt.c"
void cpp_external_print (void);
/* Arguments to the most recently seen function. */

#line 1540 "wt.c"
extern struct arg * * fargs;
/* Number of arguments to the most recently seen function. */

#line 1544 "wt.c"
extern unsigned n_fargs;
/* Size of allocated memory for `fargs'. */

#line 1548 "wt.c"
extern unsigned max_fargs;
/* Free all the memory allocated for the argument names and type
   names, then reset the counter for arguments.  This function is
   called when an argument list has been printed. */

#line 1556 "wt.c"
void argument_reset (void);
/* Allocate memory to copy text into and copy it, then make a pointer
   to it in the types array */

#line 1573 "wt.c"
void argument_save (const char * text , unsigned text_length );
/* Test if the function is an ANSI C style one with the arguments'
   types declared or not. */

#line 1587 "wt.c"
int no_prototype (void);
/* Go on to a new argument.  The last "type" was the name of an
   argument, so move it into the arg_name array and nullify the "type"
   pointer. */

#line 1604 "wt.c"
void argument_next (void);
/* Print out the list of arguments that were seen between the most
   recently seen pair of brackets `(' and `)' */

#line 1628 "wt.c"
void argument_print (void);
/* Print everything external which has been seen regardless of anything
   else. */

#line 1686 "wt.c"
void external_clear (void);
/* Print an external variable and reset everything. 

   The argument is a semicolon char or a `{'.  The semicolon is usual,
   the curly brace occurs when dealing with a named struct such as
   `struct x { ... } y, z' where Cfunctions needs to print the `x' as
   well as `y' and `z'. */

#line 1709 "wt.c"
void external_print (const char * semicolon );
/* Print something like `struct empty;'. */

#line 1753 "wt.c"
void forward_print (const char * end );
/* Free all the memory used in the most recent parse and reset all the
   switches to their default values ready for a new parse. */

#line 1765 "wt.c"
void function_reset (void);
/* Save a function type or name in the current list. */

#line 1789 "wt.c"
void function_save (const char * text , unsigned yylength );
/* Write GNU C extensions for a function. */

#line 1813 "wt.c"
void write_gnu_c_x (void);
/* Print out the current list of function types and the function
   name. */

#line 1834 "wt.c"
void function_print (void);
/* Print the name of the current source file to the standard output. */

#line 1891 "wt.c"
void print_name_stamp (const char * name );
/* Print the advert for the top of the header file. */

#line 1915 "wt.c"
void print_advert (void);
/* Print a guard wrapper for a header file.  A guard wrapper is the
   `#ifndef' construction to stop the contents of the file being seen
   twice. */

#line 1939 "wt.c"
void wrapper_top (char * h_file_name , char ** h_file_guard );
/* Write the bottom wrapper (the `#endif') of the guard wrapper for a
   `.h' file (see the comments before the function `wrapper_top' for
   an explanation).  */

#line 1990 "wt.c"
void wrapper_bottom (char * h_file_guard );
/* Check the two files for differences.  If they are the same, then
   remove the new one, and rename the backup file to what it was
   before.  */

#line 2002 "wt.c"
void unbackup (char * backup_name , char * file_name );
/* Read an input C file. */

#line 2033 "wt.c"
void read_file (void);
/* Copy a header file to a back up. */

#line 2064 "wt.c"
char * do_backup (char * file_name );
/* 
   Extract function names from a C file.
*/

#line 2088 "wt.c"
void extract (char * c_file_name );

#line 2253 "wt.c"

#ifdef CFUNCTIONS_DEBUG
void set_debug_flag (char * flag_name );

#endif /* def CFUNCTIONS_DEBUG */

#line 2333 "wt.c"
extern const char * const version_info;

#endif /* CFH_WT_H */
