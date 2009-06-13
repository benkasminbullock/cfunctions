/* Main program of Cfunctions. */

/* 
   Copyright (C) 1998, 2004, 2009  Ben K. Bullock

   Cfunctions is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   Cfunctions is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with Cfunctions; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#include <stdio.h>
#include <time.h>
#include <sys/stat.h>
#include <unistd.h>
#include <ctype.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include "cfunctionsrc.h"
#include "rc-rw.h"
#include "wt.h"
#include "print_usage.h"

/* Declarations of Flex objects. */

extern int yylex ();
extern int yylineno;
extern int yy_flex_debug;
extern FILE * yyin;

/* Helper functions to work around lack of flexibility in Flex. */

void push_in_cpp (void);
int initial_state (void);
int argument_state (void);
void start_initial (void);
void start_maybe_traditional (void);
void pop_state (void);
const char * state_message (void);

#ifdef HEADER
#undef BOOL
#define BOOL int
enum bool { FALSE, TRUE };
#endif /* HEADER */

/* The whole command line. */

static char * command_line;

/* Maximum number of brackets `(' to expect in function arguments. */

#define MAX_ARG_BR_DEPTH 3


#include "config.h"
#include "error_msg.h"
#include "sys_or_exit.h"
#include "file.h"
#include "file-name.h"
#include "tags.h"
#include "options.h"
#include "traditional.h"
#include "backup.h"
#ifndef DISABLE_CPP
#include "cpp.h"
#endif
#include "argument.h"

/* The store of information about print format.  This is for the GNU C
   extension `__attribute__((format(printf,,)))' which tells the
   compiler that the arguments to a function are in `printf' format
   and asks it to check them for errors.  */

#ifdef HEADER
/* Temporarily, this is a `tentative definition'.  The fix I will make
   is to make PRINT_FORMAT just one rule. */
struct
{
  unsigned index;
  unsigned value[2];
} 
pf;
#endif /* HEADER */

/* Files to write output to.  The file `outfile' is the usual output
   file.  The file `localfile' is the file for outputting things which
   are prefixed with `LOCAL'.  The file `verbatimfile' is the file to
   write verbatim output to.  This can be either `localfile' or
   `outfile'. */

FILE * outfile;
FILE * localfile;
FILE * verbatim_file;

/* Name of source file from `#line' directives. */

#define MAX_LINE_NAME 0x100
char line_source_name[MAX_LINE_NAME];

/* The advertisement that Cfunctions usually prints at the top of
   generated header files. */

#define CFUNCTIONS_AD \
  "/* This is a Cfunctions (version %s) generated header file.\n"\
  "   Cfunctions is a free program for extracting headers from C files.\n"\
  "   Get Cfunctions from `%s'. */\n\n"

/* Command line option-related variables.  Each variable is followed
   by the short form of the option which affects it.  See the arrays
   `usage' and `long_options' in `options.c' for explanation. */

const char * advert = CFUNCTIONS_AD;  /* -a */
BOOL advert_arg;                /* -a */
BOOL backup;                    /* -b */
BOOL copy_c_ex;                 /* -c */
BOOL c_preprocess;              /* -C */
BOOL individual;                /* -i */
BOOL c_ex_std_include;          /* -I */
BOOL keep_empty_files;          /* -k */
BOOL write_comments;            /* -m */
BOOL write_line_numbers;        /* -n */
BOOL save_static_funcs;         /* -s */
char * wrap;                    /* -w */
BOOL extensions = TRUE;         /* -x */

const char * local_macro = "LOCAL_H"; /* -L */
const char * global_macro = "HEADER"; /* -G */

static struct cfunctionsrc rc;

struct outfile
{
  char * name;
  unsigned name_len;
  char * file_name;
  char * guard_name;
  char * backup_name;
  FILE * file;
} 
global,                                 /* -g */
local;                                  /* -l */

const char *prototype_macro = "PROTO"; /* -p */

/* The wrapper for inline macros for GNU C. */

const char * inline_macro = "X_INLINE";

/* The depth of braces `{' and `}' seen. */

unsigned curly_braces_depth;

/* If `curly_braces_depth' goes beyond the following arbitrary limit
   Cfunctions gives a warning message.  Mostly this is in case
   Cfunctions itself has gone bananas.  Because most counters in
   Cfunctions are unsigned, overflow errors with large numbers
   probably mean underflows have occured.  */

#define MAX_CURLY_DEPTH 20

/* From here is the CPP stuff. */

#define MAX_CPP_IFS 200

#ifdef HEADER
/* Different kinds of preprocessor conditional statements */

typedef enum 
{
  CPP_ZAP, /* CPP_ZAP must be equal to zero, because it is used in
              tidying up `retired' stack elements. */
  CPP_IF, CPP_ELSE, CPP_ELIF, CPP_ENDIF, N_CPP_IF_TYPES
}
Cpp_If_Type;
#endif /* HEADER */

struct cpp_if
{
  Cpp_If_Type type;
  char * text;          /* This can be NULL if not `#if' or `#elif'. */
  unsigned printed : 1; /* TRUE if this has been printed */

  /* Boolean `external' is TRUE if this CPP element is outside an
     argument or function definition list.  This is set for all
     elements on which a `tidy' is performed but not for elements
     inside a function for example. */

  unsigned external : 1; 
#ifdef CFUNCTIONS_DEBUG
  unsigned print_line; /* For debugging: if printed, when was it printed ? */
#endif
}
/* The stack of CPP `#if', `#else', `#elif' and `#endif'
   statements. */

cpp_if_stack[MAX_CPP_IFS], 

/* An empty element which is copied to places in `cpp_if_stack'
   which Cfunctions wants to clean up. */

empty_cpp_if;

#ifdef CFUNCTIONS_DEBUG

/* This keeps track of how many lines have been printed by the CPP
   output part. */

unsigned cpp_prints;

#endif /* CFUNCTIONS_DEBUG */

/* The top of the `cpp_if_stack' stack. */

unsigned cpp_if_now; 

#define cpp_stack_top cpp_if_stack[cpp_if_now]

/* The top-level thing which is being parsed. */

struct arg * current_arg;

/* The number of brackets inside an argument list.  Brackets inside an
   argument list are most likely to happen for function pointer
   arguments.

   Deficiency: I am not sure where else it could happen.  There is a
   MAX variable just in case things go wrong. */

unsigned arg_br_depth;

/* The number of prototypes written to the local header file.  If this
   is zero, then Cfunctions knows that the local header file has no
   content and can be removed. */

unsigned n_local_writes;

/* The maximum number of chars allowed in a struct name. */

#define MAX_STRUCT_CHARS 0x100 

#ifdef CFUNCTIONS_DEBUG

/* Debugging flags. */

struct 
{
  unsigned func  : 1;
  unsigned cpp   : 1;
  unsigned brace : 1;
  unsigned arg   : 1;
  unsigned fptr  : 1;
  /* Comments. */
  unsigned comm  : 1;
}
cfunctions_dbug;

#endif /* CFUNCTIONS_DEBUG */

/* Are we writing an inline function?  This is not part of the parsing
   state because it needs to survive through `function_reset'. */

BOOL inlining;

/* Is Cfunctions copying everything?  (this is set by `#ifdef HEADER'
   statements) */

BOOL verbatiming;

/* Is Cfunctions copying a `.hin' file into the header file? */

BOOL hin_copying;

/* The parsing state information.  `function_reset' resets this to all
   zeros. */

struct cfunctions_parse_state
{
  /* The number of function arguments seen so far. */

  unsigned function_type_n;

  /* set to VOID only if the function does not have a return value */

  enum { RETURN, VOID } c_return_value;

  /* Have we seen `extern'?  This is needed to extract global variables
     correctly (`extern' global variables are ignored by Cfunctions). */

  unsigned seen_extern;

  /* Have we seen `static'? */

  unsigned seen_static : 1;

  /* Have we seen the `inline' keyword? */

  unsigned seen_inline : 1;

  /* TRUE if function has no side effects. */

  unsigned no_side_effects : 1;

  /* TRUE if function arguments have been seen */

  unsigned seen_arguments : 1;

  /* TRUE if the function never returns (for example `exit'). 
     This is set by `NO_RETURN'. */

  unsigned c_no_return : 1 ;

  /* TRUE if PRINT_FORMAT argument was seen.  */

  unsigned seen_print_format : 1;

  /* FALSE if this is a user function (in other words a global function)
     in a library consisting of several files */

  unsigned local_func : 1;

  /* TRUE if typedef was seen. */

  unsigned seen_typedef : 1;

  /* TRUE if struct with no name and not static, not in typedef. */

  unsigned unnamed_struct : 1;

  /* TRUE if this function is a traditional C function. */

  unsigned is_trad : 1;

  /* TRUE if Cfunctions saw a word after a `struct', `union' or `enum'
     statement. */

  unsigned saw_word : 1;

  /* TRUE for `(void)' functions. */

  unsigned void_arguments : 1;
}

/* The state information is kept in `s'.  The instance `z' is kept
   empty and used just for copying to reset the state.  Doing this via
   struct copying makes the code simpler and means that Cfunctions
   never forgets to reset all of the states at the end of parsing a
   function or external variable. */

s, z;

/* Are we reading from standard input? */

BOOL reading_from_stdin;

/* Are we writing to standard output? */

BOOL writing_to_stdout;

/* Line number of rule matched. */

unsigned rule_line;

/* Warnings. */

#ifdef HEADER
struct warning
{
  unsigned implicit_int      : 1;
  unsigned strict_prototypes : 1;
  unsigned reserved_words    : 1;
};
#endif /* HEADER */

struct warning warns;

/* Process the command line argument. */

static void
process_warning (const char * warning_arg)
{
  if (strcmp (warning_arg, "implicit-int") == 0
      || strcmp (warning_arg, "implicit") == 0)
    {
      warns.implicit_int = 1;
      return;
    }
  if (strcmp (warning_arg, "strict-prototypes") == 0)
    {
      warns.strict_prototypes = 1;
      return;
    }
  if (strcmp (warning_arg, "reserved") == 0)
    {
      warns.reserved_words = 1;
      return;
    }
  warning ("unknown argument `%s' to -W", warning_arg);
}

static void
print_line_number (void)
{
  if (write_line_numbers)
    fprintf (outfile, "\n#line %u \"%s\"\n", yylineno, source_name);
}

/* Check whether a particular quantity has overflowed a maximum, and if
   so issue a warning. */

static void 
check_overflow (unsigned n, const unsigned max, const char * name)
{
  if (n > max)
    line_warning ("possible overflow of %s: %u > %u",
                  name, n, max);
}

/* count a `{'. */

void
brace_open (void) 
{
  curly_braces_depth++; 
#ifdef CFUNCTIONS_DEBUG
  if (cfunctions_dbug.brace) 
    printf ("%s:%u: brace debug: `{' (%d deep) at %s:%u\n", 
            source_name, yylineno, curly_braces_depth, __FILE__, 
            __LINE__ ); 
  check_overflow (curly_braces_depth, MAX_CURLY_DEPTH, "curly braces");
#endif /* CFUNCTIONS_DEBUG */
}

/* count a `}'. */

void
brace_close (void)
{
  curly_braces_depth--; 
#ifdef CFUNCTIONS_DEBUG
  if (cfunctions_dbug.brace) 
    printf ("%s:%u: brace debug: `}' (%d deep)\n", 
            source_name, yylineno, curly_braces_depth); 
  check_overflow (curly_braces_depth, MAX_CURLY_DEPTH, "curly braces");
#endif /* CFUNCTIONS_DEBUG */
}

/* Comments. */

static BOOL doing_comment;
static char * comment_buffer;
static unsigned comment_buf_len;
static unsigned comment_len;

/* Initial length of the comment buffer. */

#define INITIAL_LEN 0x100

/* Add some more comment to the comment buffer. */

static void
comment_add (const char * text, unsigned leng)
{
#ifdef CFUNCTIONS_DEBUG       
  if (cfunctions_dbug.comm)
    line_info ("comment add %s\n", text);
#endif
  if (! comment_buffer)
    {  
      comment_buffer = malloc_or_exit (INITIAL_LEN);
      comment_buf_len = INITIAL_LEN;
    }
  if (comment_buf_len <= comment_len + leng)
    {
      comment_buf_len = comment_len + leng + 1;
      comment_buffer = realloc_or_exit (comment_buffer, comment_buf_len);
    }
  if (comment_len)
    strcat (comment_buffer, text);
  else
    strcpy (comment_buffer, text);
  comment_len += leng;
}

void
do_comment_start (void)
{
  if (write_comments && initial_state ())
    {
#ifdef CFUNCTIONS_DEBUG       
      if (cfunctions_dbug.comm)
        line_info ("comment start");
#endif
      comment_len = 0;
      doing_comment = 1;
      comment_add ("/*", 2);
    }
}

void
do_comment_end (void)
{
  if (doing_comment)
    {
#ifdef CFUNCTIONS_DEBUG       
      if (cfunctions_dbug.comm)
        line_info ("comment end");
#endif  
      comment_add ("*/\n", 3);
    }
  doing_comment = 0;
}

void
do_comment_print (const char * text, int leng)
{
  if (doing_comment)
    {
      comment_add (text, leng);
    }
}

static void
comment_reset (void)
{
#ifdef CFUNCTIONS_DEBUG       
  if (cfunctions_dbug.comm)
    line_info ("comment reset");
#endif
  comment_len = 0;
}

static void
print_comment (void)
{
#ifdef CFUNCTIONS_DEBUG       
      if (cfunctions_dbug.comm)
        line_info ("print comment");
#endif  
  if (comment_len)
    fprintf (outfile, "%s", comment_buffer);
  comment_reset ();
}

/* The name do_PRINT_FORMAT is misleading: this is a temporary fix. */

void
do_PRINT_FORMAT (void)
{
  s.seen_print_format = TRUE;
  start_initial ();
}

void
do_void_pointer (const char * text)
{
  char * c;
 /* `void *' functions have a return value */
  function_save ("void", 4 );
  c = strchr (text, '*');
  function_save (c, strlen (c));
}


void
do_start_arguments (void)
{
  argument_next ();
  arg_put_name (current_arg);
  s.seen_arguments = TRUE;
}

void
do_arguments (void)
{
  arg_put_name (current_arg);
  s.seen_arguments = TRUE;
}

void
do_function_pointer (const char * text)
{
  inline_print (text);
#ifdef CFUNCTIONS_DEBUG
  if (cfunctions_dbug.fptr)
    printf ("pointer to function `%s'\n", text);
#endif
}


void
do_word (const char * text, int leng)
{
  if (s.saw_word)
    pop_state ();
  function_save (text, leng);
  s.saw_word = TRUE;
}

void
do_typedef (const char * text, int leng)
{
  if (verbatiming)
    function_save (text, leng);
  else
    s.seen_typedef = TRUE;
}

void
inline_print (const char * x) 
{
  if (inlining) 
    fprintf (outfile, "%s", x); 
  else if (verbatiming) 
    fprintf (verbatim_file, "%s", x);
}

/* Deal with preprocessor `#line' directives. */

void
line_change (const char * text)
{
  char * first_quote;
  unsigned name_length;
  char * line_at;
  first_quote = strchr (text, '\"' );
  name_length = strlen (first_quote);
#ifdef CFUNCTIONS_DEBUG
  if (name_length > MAX_LINE_NAME)
    {
      line_warning ("very long source name `%s': truncating",
                first_quote);
      strncpy (line_source_name, first_quote + 1, 
               MAX_LINE_NAME);
      line_source_name[MAX_LINE_NAME-1] = '\0';
    }
  else
#endif
    {
      strcpy (line_source_name, first_quote + 1);
      line_source_name[name_length-2] = '\0';
    }
  line_at = strstr (text, "line");
  if ( ! line_at )
    {
      line_at = strstr (text, "#");
#ifdef CFUNCTIONS_DEBUG
      if ( ! line_at )
            bug (HERE, "peculiar mismatch in strstr: "
                 "no `line' or `#' in `%s'", text);
#endif
      line_at += 1;
    }
  else
    line_at += 4;

  yylineno = atoi (line_at) - 1;
  source_name = line_source_name;
  tag_new_file (source_name);
  push_in_cpp ();
}


/* Fix for Flex bug which causes wrong line numbers.  The problem is
   the `$' in the rule for matching C preprocessor stuff.  These rules
   have a dollar to match end of line.  The problem arises because
   Flex actually counts the `\n' which comes after the dollar when it
   is calculating yylineno, and then it `rejects' the `\n', then
   because it was rejected it counts it again.  I have sent the full
   description of the bug to Vern Paxson, the author of Flex, who said
   it is on the `to do' list. */

/* I am hoping that the bug will be fixed by version 2.6 of Flex. */

#if YY_FLEX_MAJOR_VERSION <= 2 && YY_FLEX_MINOR_VERSION <= 5
#define FIX_FOR_FLEX_BUG
#else
#ifndef FLEX_BUG_WARNING_OFF
#ifdef HAS_WARNING
#warning "This file contains a fix for a bug in"
#warning "Flex.  The fix has been turned off"
#warning "for Flex version > 2.5.  If you still"
#warning "find Cfunctions gets line numbers"
#warning "wrong, please report the bug to the "
#warning "Cfunctions maintainer and the Flex "
#warning "maintainer.  Use"
#warning "`#define FLEX_BUG_WARNING_OFF' to stop"
#warning "seeing this message next time."
#endif /* HAS_WARNING */
#endif /* FLEX_BUG_WARNING_OFF */
#endif /* new version of Flex */

/* Cfunctions sometimes needs to include `c-extensions.h' because it
   defines all the information about C extensions. */

void
copy_c_extensions (void)
{
  static BOOL c_ex_searched = FALSE;
  static const char * c_ex_file_name = NULL;

  if ( ! c_ex_searched )
    {
      c_ex_searched = TRUE;

      /* This could be replaced by a PATH based search. */

      if ( fexists ( C_EXTENSIONS_FILE ) )
        c_ex_file_name = C_EXTENSIONS_FILE;
      else if ( fexists ( C_EX_LOCAL ))
        c_ex_file_name = C_EX_LOCAL;
      else
        warning ( "cannot find master copy of `%s'", C_EX_FILE_NAME );
    }

  if ( copy_c_ex )
    {
      if ( c_ex_file_name )
        {
          fprintf (outfile, "#line 1 \"%s\"\n", c_ex_file_name);
          if ( fcopy (outfile, c_ex_file_name ) )
            warning ( "could not copy `%s': %s", c_ex_file_name, 
                      strerror (errno));
        }
    }
  else
    {
      if (c_ex_std_include)
        {
          fprintf ( outfile, "#include <"C_EX_FILE_NAME">\n" );
          return;
        }
      else
        {
          fprintf ( outfile, "#include \""C_EX_FILE_NAME"\"\n" );
        } 
      if (! extensions)
        return;

      if ( c_ex_file_name 
           && ( ! fexists ( C_EX_FILE_NAME ) 
                || fdiff ( C_EX_FILE_NAME, c_ex_file_name ))
           && ! writing_to_stdout )
        {
          FILE * c_ex;

          c_ex = fopen_or_exit ( C_EX_FILE_NAME, "w" );
          if ( fcopy (c_ex, c_ex_file_name ) ) 
            warning ( "could not copy `%s': %s", c_ex_file_name,
                      strerror (errno));
          fclose (c_ex);
        }
    }
}

static unsigned written_c_extensions;

/* Check whether the user has asked for the C extensions, which are
   turned off by default.  If the user needs them, then write a
   statement #include "c-extensions.h".  The user needs the extensions
   file also if he uses traditional C declarations, because the
   definition of a macro `PROTO' for the traditional C declarations is
   in there.  */

void 
check_extensions (void)
{
  if (! extensions || written_c_extensions) 
    return;
  copy_c_extensions ();
  written_c_extensions = 1;
}

/* Names of various preprocessor conditions.  These are used to find
   substrings in a Cpp statement. "ZAP" means mark for destruction. */

static const char * cpp_if_names[N_CPP_IF_TYPES] = 
{
  "ZAP", "if", "else", "elif", "endif"
};

/* The string lengths of `cpp_if_names'. */

static unsigned cpp_if_len[N_CPP_IF_TYPES] = 
{
  3, 2, 4, 4, 5
};

/* 
   Given a `#endif', find the matching `#if'.

   Return value: stack position of matching `#if'.  

   Side effects: If this routine cannot find the matching `#if' it
   aborts with an error message.

   Deficiency: this routine is a duplicate of code in the
   cpp_stack_tidy routine.  
*/

static int
cpp_stack_find_if (int i)
{
  unsigned endif_level = 1;
  int depth;

  depth = i - 1; 

  while (1)
    {
      if (depth < 0)
        line_error ("can't find matching #if");
#ifdef CFUNCTIONS_DEBUG
      if (cfunctions_dbug.cpp)
        printf ("Looking at depth %d: type is `%s', string is `%s'\n", depth, 
                cpp_if_names[cpp_if_stack[depth].type], 
                cpp_if_stack[depth].text ? cpp_if_stack[depth].text : "empty");
#endif
      switch (cpp_if_stack[depth].type)
        {
        case CPP_IF:
          endif_level--;
          if (! endif_level)
            return depth;
          break;
        case CPP_ENDIF:
          endif_level++;
          break;
        case CPP_ELSE:
        case CPP_ELIF:
        case CPP_ZAP:
          break;
        default:
#ifdef CFUNCTIONS_DEBUG
          bug ( HERE, "bad number %d in switch", 
                cpp_if_stack[depth].type );
#endif
          ;
        }
      depth--;
    }
}

/* Stack position of an `#if' statement which started a `verbatim
   copying' routine. */

static unsigned verbatim_limit;

#ifdef CFUNCTIONS_DEBUG

/* Debugging function to show `cpp_if_stack'.  */

static void
show_cpp_if_stack(unsigned i)
{
  printf ( "CPP debug: cpp_if_stack[%d] = #%s%s\n", i,
           cpp_if_names[cpp_if_stack[i].type], 
           cpp_if_stack[i].text );
}

#endif /* CFUNCTIONS_DEBUG */

/* Pass through the CPP stack and move everything to fill in holes. */

static void
cpp_fill_holes (void)
{
  unsigned i, j = 0;

#if 0
  printf ("HOLES BEFORE: ");

  for (i = 0; i < cpp_if_now; i++)
    {
      printf ("%s ", cpp_if_names[cpp_if_stack[i].type]);
    }
  printf ("\n");
#endif /* 0 */
  for (i = 0; i < cpp_if_now; i++)
    {
      if ( cpp_if_stack[i].type != CPP_ZAP )
        {
          if (i != j)
            {
              cpp_if_stack[j] = cpp_if_stack[i];
              if ( verbatim_limit && i == verbatim_limit )
                {
#ifdef CFUNCTIONS_DEBUG
                  if (cfunctions_dbug.cpp)
                    printf ("CPP debug: lowering verbatim limit "
                            "from %d to %d\n", i, j);
#endif
                  verbatim_limit = j;
                }
            }
          j++;
        }
      else
        free (cpp_if_stack [ i ].text);
    }

  /* Reset the top of the stack. */

  for ( i = j; i < cpp_if_now; i++ )
    cpp_if_stack[i] = empty_cpp_if;
    
  cpp_if_now = j;
    
#ifdef CFUNCTIONS_DEBUG
  if (cfunctions_dbug.cpp)
    printf ( "CPP debug: stack size after tidying %d\n", cpp_if_now );
#endif
#if 0
  printf ("HOLES AFTER: ");

  for (i = 0; i < cpp_if_now; i++)
    {
      printf ("%s ", cpp_if_names[cpp_if_stack[i].type]);
    }
  printf ("\n");
#endif /* 0 */
}

/* 
   Find all matching #if and #endif pairs in `cpp_if_stack' and remove
   them, then move all the remaining entries downwards.
*/

static void
cpp_stack_tidy (void)
{
  int i;

  /* Find matching pairs of `#if' and `#endif' and mark them for
     deletion. */

  for (i = cpp_if_now - 1; i >= 0; i--)
    {
      if (cpp_if_stack[i].type == CPP_ENDIF)
        {
          /* Cfunctions has found an `#endif', and it will now descend
             the stack looking for the partner `#if'. */

          unsigned endif_level = 1;
          int depth;
          BOOL printed = cpp_if_stack[i].printed;

          depth = i - 1; 

#ifdef CFUNCTIONS_DEBUG
          if (cfunctions_dbug.cpp)
            {
              printf ( "CPP debug: zapping #endif %s\n", 
                       cpp_if_stack[i].text );
              if (printed)
                printf ("CPP debug: already printed at %u.\n",
                        cpp_if_stack[i].print_line);
              else
                printf ("CPP debug: not printed.\n");
            }
#endif
          while (1)
            {
              if (depth < 0)
                line_error ("too many `#endif's");
#ifdef CFUNCTIONS_DEBUG
              if (cfunctions_dbug.cpp)
                show_cpp_if_stack (depth);
#endif
              switch (cpp_if_stack[depth].type)
                {
                case CPP_IF:
                  endif_level--;
                  if (! endif_level)
                    {
                      if (! printed && cpp_if_stack[depth].printed)
                        cpp_eject (i);
#ifdef CFUNCTIONS_DEBUG
                      else if ( printed && ! cpp_if_stack[depth].printed)
                        bug (HERE, "#endif printed but #if not printed");
                      else 
                        if (cfunctions_dbug.cpp)
                          {
                            if (! cpp_if_stack[depth].printed)
                              printf ("CPP debug: #if was not printed\n");
                          }
#endif
                      cpp_if_stack[depth].type = CPP_ZAP;
#ifdef CFUNCTIONS_DEBUG
                      if (cfunctions_dbug.cpp)
                        printf ("CPP debug: zapping `#if %s'\n", 
                                cpp_if_stack[depth].text);
#endif
                      goto zapped;
                    }
                  break;
                case CPP_ENDIF:
                  endif_level++;
                  break;
                case CPP_ELIF:
                case CPP_ELSE:
                  if (endif_level == 1)
                    {
                      cpp_if_stack[depth].type = CPP_ZAP;
                      #ifdef CFUNCTIONS_DEBUG
                      if (cfunctions_dbug.cpp)
                        {
                          printf ("CPP debug: zapping #else/#elif \"%s\"\n", 
                                  cpp_if_stack[depth].text);
                          if (! cpp_if_stack[depth].printed)
                            printf ("CPP debug: not printed\n");
                        }
                      #endif
                    }
                  break;
                case CPP_ZAP:
                  break;
                default:
#ifdef CFUNCTIONS_DEBUG
                  bug (HERE, "bad number %d at depth %d in switch", 
                       cpp_if_stack[depth].type, depth);
#endif
                  ;
                }
              depth--;
            }
        zapped:
          /* Cfunctions has found a matching `#if' and `#endif' pair. */
          cpp_if_stack[i].type = CPP_ZAP;
        }
    }
  cpp_fill_holes ();

  /* Set everything to `external'.  Now Cfunctions knows that the C
     preprocessor stuff currently on the stack is not entangled with
     the currently parsed function or variable. */

  for (i = 0; i < cpp_if_now; i++)
    if (! cpp_if_stack[i].printed)
      cpp_if_stack [i].external = 1;
}

/* Push a new thing onto the preprocessor conditional stack. */

void
cpp_add (char * text, Cpp_If_Type type)
{
  char * x;
  unsigned leng;
  char cpp_word[9];

#ifdef FIX_FOR_FLEX_BUG
  yylineno--;
#endif

#ifdef CFUNCTIONS_DEBUG
  if (cfunctions_dbug.cpp)
    printf ( "CPP debug: function \"cpp_add\":"
             "saving `%s' of type `%s' from line %u\n", 
             text, cpp_if_names[type], yylineno );
#endif

  x = strstr (text, cpp_if_names[type]);

#ifdef CFUNCTIONS_DEBUG
  if ( ! x )
    bug ( HERE, "bad string `%s' in cpp_add: should contain `%s'", text,
          cpp_if_names[type]);
#endif

  x += cpp_if_len[type];
  leng = strlen(x);

  if (leng)
    {
      cpp_stack_top.text = malloc_or_exit ( leng + 1 );
      strcpy ( cpp_stack_top.text, x );
    }
  else
    cpp_stack_top.text = NULL;
  cpp_stack_top.type = type;
  cpp_stack_top.printed = FALSE;

  /* Check for the verbatim copying conditions. */

  if (type == CPP_IF)
    {
      if ( ! verbatiming )
        {
          if ( strstr (x, local_macro) )
            {
              verbatiming = TRUE;
            }
          else if ( strstr (x, global_macro) )
            {
              if ( global.file )
                outfile = global.file;
              verbatiming = TRUE;
            }
          if (verbatiming)
            {
              verbatim_file = outfile;
              verbatim_limit = cpp_if_now;
              cpp_stack_top.printed = TRUE;
              print_line_number ();
              goto verbatim_started;
            }
        }
    }

  if (verbatiming)
    {
#ifdef CFUNCTIONS_DEBUG
      if (! hin_copying)
        {
          if ( cfunctions_dbug.cpp )
            {
              printf ("CPP debug: looking for end of verbatim\n");
              printf ("%d %d\n", cpp_stack_find_if (cpp_if_now), 
                      verbatim_limit);
            }
        }
#endif
      if ( type == CPP_ENDIF && ! hin_copying && 
           cpp_stack_find_if (cpp_if_now) == verbatim_limit)
        {
          /* Cfunctions has hit the final `#endif' of a verbatim copying
             area. */
#ifdef CFUNCTIONS_DEBUG
          if ( cfunctions_dbug.cpp )
            printf ("CPP debug: final `#endif' of a verbatim area\n");
#endif
          cpp_stack_top.printed = TRUE;
          cpp_stack_tidy ();
          verbatiming = FALSE;
        }
      else
        {
          outfile = verbatim_file;
          cpp_eject (cpp_if_now);
        }
    }
  else
    {
      /* The following string acts as a marker to other routines that they
         should call `cpp_eject' with the number printed at the end of the
         string.

         Each client routine has to call `atoi' to get the number and call
         `cpp_eject'. */

      leng = sprintf ( cpp_word, "@CPP%u", cpp_if_now);
      
      if (initial_state ())
        {
#ifdef CFUNCTIONS_DEBUG
          if (cfunctions_dbug.func)
            printf ("initial state\n");
#endif

          function_save (cpp_word, leng);
        }
      else if (argument_state ())
        argument_save (cpp_word, leng);
      else
        {
          /* Even if the statement is ignored here, it remains on the
             stack until the next call to `cpp_stack_tidy'.  The `printed'
             flag in `cpp_ifstack' ensures that an `#endif' will be
             printed if a corresponding `#if' has been emitted. */
          ; 
        }
    }
  
 verbatim_started:

  cpp_if_now++;

#ifdef CFUNCTIONS_DEBUG
  if (cfunctions_dbug.cpp)
    printf ( "CPP debug: if stack now %u deep\n", cpp_if_now );
#endif

  /* Deficiency: there is no way to resize the stack */

  if (cpp_if_now > MAX_CPP_IFS)
    line_error ("too many `#if's: limit is %d", MAX_CPP_IFS );
}

/* Write out the preprocessor statement on the stack at position `u'.  */

void
cpp_eject (unsigned u)
{
#ifdef CFUNCTIONS_DEBUG
  if (cpp_if_stack[u].printed)
    {
      unsigned j;
      for (j = 0; j < cpp_if_now; j++ )
        show_cpp_if_stack(j);
      bug (HERE, "Attempt to eject already printed statement %d `%s'", 
           u, cpp_if_stack[u].text);
    }
#endif

  if (cpp_if_stack[u].type == CPP_ZAP)
    return;

  /* The following one might point `outfile' to the global file
     wrongly when parsing local files. */

  if (global.file)
    outfile = global.file;

  /* Start a new line. */

  fprintf (outfile, "\n");
    
  if (cpp_if_stack[u].type == CPP_ENDIF)
    {
      int matching_if;

      matching_if = cpp_stack_find_if (u);

      /* Cfunctions currently prints almost every conditional so the
         following is not executed. */

      if (! cpp_if_stack [matching_if].printed)
        {
          #ifdef CFUNCTIONS_DEBUG
          if (cfunctions_dbug.cpp)
            printf ( "CPP debug: `#endif' but matching `#if' not printed\n" );
          #endif
          return;
        }
      /* Deficiency: depending on the previous statement, this sometimes
         prints too many `\n's.  If the last statement ended with `\n'
         there will be two consecutive `\n's */

      if (cpp_if_stack[u].text)
        fprintf (outfile, "#%s%s\n", cpp_if_names[cpp_if_stack[u].type],
                 cpp_if_stack[u].text );
      else
        fprintf (outfile, "#%s\n", cpp_if_names[cpp_if_stack[u].type] );
    }
  else if (cpp_if_stack[u].text)
    {   
      fprintf (outfile, "#%s%s\n", cpp_if_names[cpp_if_stack[u].type],
               cpp_if_stack[u].text); 
    }
  else
    {
#ifdef CFUNCTIONS_DEBUG
      if ( cpp_if_stack[u].type == CPP_IF ||
           cpp_if_stack[u].type == CPP_ELIF )
        bug (HERE, "%s with no condition", 
             cpp_if_names [cpp_if_stack[u].type]);
#endif
      fprintf (outfile, "#%s\n", cpp_if_names [cpp_if_stack[u].type]);
    }
  cpp_if_stack[u].printed = TRUE;

#ifdef CFUNCTIONS_DEBUG

  /* Print a line number to indicate where a particular statement
     was printed. */

  cpp_if_stack[u].print_line = cpp_prints++;
  if (cfunctions_dbug.cpp)
    printf ("CPP debug: print line %u\n", cpp_if_stack[u].print_line);
#endif
}

void 
do_start_cpp (const char * text)
{
  if (initial_state ())
    function_reset ();
  push_in_cpp (); 
  inline_print (text); 
}

void
do_escaped_brace (const char * text)
{
  inline_print (text);
#ifdef CFUNCTIONS_DEBUG
  if (cfunctions_dbug.brace)
    printf ("%s:%u: matched escaped brace.\n",
            source_name, yylineno);
#endif
}

void
do_extern (const char * text, int leng)
{
  if (verbatiming)
    function_save (text, leng);
  else
    s.seen_extern = TRUE;
}

void
do_NO_RETURN (const char * text)
{
  check_extensions ();
  s.c_return_value = VOID;
  s.c_no_return = TRUE;
  function_save ("void", 5 );
}


void
do_arguments_close_bracket (const char * text, int leng)
{
  arg_br_depth--;
  if ( ! arg_br_depth )
    {
      if (no_prototype ())
        {
          s.is_trad = TRUE;
          start_maybe_traditional ();
        }
      else
        start_initial ();
    }
  else
    {
      check_overflow (arg_br_depth, MAX_ARG_BR_DEPTH, "brackets");
      argument_save (text, leng); 
    } 
}

void
do_LOCAL (const char * text)
{
  check_extensions ();
  n_local_writes++;
  s.local_func = TRUE;
}

void
do_INLINE (const char * text)
{
  check_extensions ();
  if (verbatiming)
    line_error ("INLINE inside verbatim region");
  s.seen_inline = TRUE;
  inlining = TRUE;
}

void
do_static (const char * text, int leng)
{
  s.seen_static = TRUE;
  function_save (text, leng);
}

void
do_void (const char * text, int leng)
{
  s.c_return_value = VOID;
  function_save (text, leng);
}

void
do_NO_SIDE_FX (const char * text)
{
  check_extensions ();
  s.no_side_effects = TRUE;
}


void
do_arguments_open_bracket (const char * text, int leng)
{
  arg_br_depth++;
  check_overflow (arg_br_depth, MAX_ARG_BR_DEPTH, "brackets");
  argument_save (text, leng);
}

void
do_brace_close (void)
{
  brace_close ();
  if (curly_braces_depth == 0)
    {
      if (inlining)
        {
          FILE * outfile_now = outfile;
          fprintf (outfile, "}");
          fprintf (outfile, "\n#else /* not def %s */\n", inline_macro);
          inlining = FALSE;
          
          /* Because Cfunctions was copying an inline function, the
             first call to `function_print' did not call
             `function_reset', so that Cfunctions can call
             `function_print' again here.  

             Unfortunately this resets `outfile' to `localfile'. */
          
          function_print ();
          fprintf (outfile_now, "\n#endif /* def %s */\n", inline_macro);
        }
      else if (verbatiming)
        fprintf (verbatim_file, "} ");
      pop_state ();
      if (verbatiming && initial_state ())
        fprintf (verbatim_file, "\n");
    }
  else
    inline_print ("}");
}

/* Handle `(void)' argument declarations. */

void
do_void_arguments (void)
{
  s.void_arguments = TRUE;
  do_arguments ();
}


void 
do_define (const char * text)
{
  unsigned char * macro_name;
  if (initial_state ())
    function_reset ();
  macro_name = (unsigned char *) strstr (text, "define") + 7;
  while ( !(isalnum (macro_name[0]) || macro_name[0] == '_'))
    macro_name++;
  push_in_cpp ();
  inline_print (text);
  tag_make (macro_name, TAG_MACRO, yylineno);
}

/* Nullify elements of the stack and release their memory. */

void
cpp_stack_free (unsigned p)
{
  if (cpp_if_stack[p].text)
    free (cpp_if_stack[p].text);
  cpp_if_stack[p] = empty_cpp_if;
}

/* Zap pairs of #if and #endif where #if is `external'. */

void
cpp_external_tidy (void)
{
  int i;

  /* Find matching pairs of `#if' and `#endif' and mark them for
     deletion. */

  for (i = cpp_if_now - 1; i >= 0; i--)
    {
      if (cpp_if_stack[i].type == CPP_ENDIF)
        {
          /* Cfunctions has found an `#endif', and it will now descend
             the stack looking for the partner `#if'. */

          unsigned endif_level = 1;
          int depth; 

          for (depth = i - 1; depth >= 0; depth--)
            {
              switch (cpp_if_stack[depth].type)
                {
                case CPP_ENDIF:
                  endif_level++;
                  break;
                case CPP_IF:
                  endif_level--;
                  if (! endif_level && cpp_if_stack[depth].external)
                    {
                      /* Cfunctions will delete everything on the
                         stack from this statement onwards to i. */
                      unsigned j;
                      if (cpp_if_stack[depth].printed)
                        cpp_eject (i);
                      for (j = depth; j <= i; j++)
                        cpp_stack_free (j);
                    }
                  break;
                default:
                  break;
                }
            }
        }
    }
}

/* Print out any CPP things which have not yet been printed but which
   are not part of a complete #if and #endif block. */

void
cpp_external_print (void)
{
  unsigned i;

#ifdef CFUNCTIONS_DEBUG
  if (cfunctions_dbug.cpp)
    printf ( "CPP debug: doing external print\n");
#endif

  cpp_external_tidy ();

  for (i = 0; i < cpp_if_now; i++)
    {
#ifdef CFUNCTIONS_DEBUG
      if (cfunctions_dbug.cpp)
        printf ( "CPP debug: external print of %u\n", i);
#endif

      if (cpp_if_stack[i].external && ! cpp_if_stack[i].printed)
        cpp_eject (i);
#ifdef CFUNCTIONS_DEBUG
      else if (cfunctions_dbug.cpp)
        printf ( "CPP debug: not external so rejected\n");
#endif
    }
}

/* Arguments to the most recently seen function. */

struct arg ** fargs;

/* Number of arguments to the most recently seen function. */

unsigned n_fargs;

/* Size of allocated memory for `fargs'. */

unsigned max_fargs;

/* Free all the memory allocated for the argument names and type
   names, then reset the counter for arguments.  This function is
   called when an argument list has been printed. */

void
argument_reset(void)
{
  unsigned i;

#ifdef CFUNCTIONS_DEBUG
  if (cfunctions_dbug.arg)
    printf ("argument reset.\n");
#endif
  for (i = 0; i < n_fargs; i++)
    arg_free (fargs[i]);
  n_fargs = 0;
}

/* Allocate memory to copy text into and copy it, then make a pointer
   to it in the types array */

void
argument_save (const char * text, unsigned text_length )
{
#ifdef CFUNCTIONS_DEBUG
  if (cfunctions_dbug.arg)
    printf ( "%s:%u: saving argument `%s' to slot %d\n", 
             source_name, yylineno, text, n_fargs - 1);
#endif /* CFUNCTIONS_DEBUG */
  arg_add (fargs[n_fargs - 1], text, 0);
}

/* Test if the function is an ANSI C style one with the arguments'
   types declared or not. */

int
no_prototype (void)
{
  unsigned i;
  for (i = 0; i < n_fargs; i++)
    {
      struct arg * a = fargs[i];
      if (! a->types->t || a->types->t->prev || a->name || a->pointers)
        return 0;
    }
  return 1;
}

/* Go on to a new argument.  The last "type" was the name of an
   argument, so move it into the arg_name array and nullify the "type"
   pointer. */

void
argument_next (void)
{
  n_fargs++;
  if (n_fargs > max_fargs)
    {
      if (fargs)
        {
          max_fargs *= 2;
          fargs = (struct arg **)
            realloc_or_exit (fargs, sizeof (struct arg *) * max_fargs);
        }
      else
        {
          max_fargs = 4;
          fargs = malloc_or_exit (sizeof (struct arg *) * max_fargs);
        }
    }
  fargs[n_fargs - 1] = arg_start ();
}

/* Print out the list of arguments that were seen between the most
   recently seen pair of brackets `(' and `)' */

void
argument_print (void)
{
  unsigned i;
  int trad;
  trad = s.is_trad && ! (inlining || verbatiming);

#ifdef CFUNCTIONS_DEBUG
  if (cfunctions_dbug.arg)
    printf ("printing argument\n");
#endif 

  fprintf (outfile,  " ");
  if (trad)
    {
      if (warns.strict_prototypes)
        line_warning ("function with no prototype");
      fprintf (outfile,  " %s (", prototype_macro );
    }
  if (n_fargs)
    {
      fprintf (outfile,  "(" );
      
      for ( i = 0; i < n_fargs; i++ )
        {
          if (trad)
            traditional_print_type (outfile, (char *) fargs[i]->types->t->name);
          else
            arg_fprint (outfile, fargs[i]);
          
          if (i < n_fargs - 1)
            fprintf (outfile, ", ");
        }
      if (trad)
        fprintf (outfile,  "))" );
      else
        fprintf (outfile, ")");
    }
  else
    {
      if (s.void_arguments)
        fprintf (outfile, "(void)");
      else 
        {
          if (warns.strict_prototypes)
            line_warning ("function with no prototype");
          /* Cfunctions is a prototype fascist. */
          fprintf (outfile, "PROTO ((void))");
        }
    }

  if ( ! inlining )
    argument_reset ();
}

/* Print everything external which has been seen regardless of anything
   else. */

void
external_clear (void)
{
  if (verbatiming)
    {
      arg_fprint (outfile, current_arg);
      fprintf (outfile, "{");
    }
  else if ( ! (s.seen_static || s.seen_typedef ))
    s.unnamed_struct = TRUE;
  if (verbatiming || ! s.seen_static || save_static_funcs)
    arg_tagable (current_arg);
  arg_free (current_arg);
  current_arg = arg_start ();
}

/* Print an external variable and reset everything. 

   The argument is a semicolon char or a `{'.  The semicolon is usual,
   the curly brace occurs when dealing with a named struct such as
   `struct x { ... } y, z' where Cfunctions needs to print the `x' as
   well as `y' and `z'. */

void
external_print (const char * semicolon)
{
  int printable;

  printable = ! s.seen_static;

  if (inlining)
    bug ( HERE, "an external variable cannot be `inline'");

  if ( verbatiming || ( ! (s.seen_arguments || s.seen_extern) && 
                        ! s.seen_typedef &&
                        ! s.unnamed_struct ) )
    {
      if (! s.local_func)
        if (global.file)
          outfile = global.file;
      if (verbatiming || printable)
        {
          print_comment ();
          print_line_number ();
          cpp_external_print ();
          if (current_arg)
            {
              arg_fprint_all (outfile, current_arg, ! verbatiming);
              if (! s.seen_arguments)
                arg_tag (current_arg, TAG_GLOBAL);
              if (verbatiming)
                if (n_fargs)
                  argument_print ();
            }
          fprintf (outfile, "%s", semicolon);
        }
      else if (save_static_funcs)
        if (! s.seen_arguments)
          arg_tag (current_arg, TAG_GLOBAL);
    }
  else if (s.seen_typedef)
    arg_tag (current_arg, TAG_TYPEDEF);
  function_reset ();
}

/* Print something like `struct empty;'. */

void
forward_print (const char * end)
{
  if (verbatiming)
    external_print (end);
  else                  
    function_reset ();
}

/* Free all the memory used in the most recent parse and reset all the
   switches to their default values ready for a new parse. */

void 
function_reset (void)
{
#ifdef CFUNCTIONS_DEBUG
  if (cfunctions_dbug.func)
    printf ("function reset\n");
#endif

  if (current_arg)
    arg_free (current_arg);
  current_arg = NULL;
  s = z;
  cpp_stack_tidy ();
  if (verbatiming)
    outfile = verbatim_file;
  else
    outfile = localfile;
  traditional_reset ();
  argument_reset ();
  comment_reset ();
}

/* Save a function type or name in the current list. */

void 
function_save (const char * text, unsigned yylength )
{

#ifdef CFUNCTIONS_DEBUG
  if (cfunctions_dbug.func)
    printf ("%s:%u: saving function word `%s'\n%s:%u: word appears here\n", 
            "cfunctions.fl", rule_line, text, source_name, yylineno );
#endif

  if (! current_arg)
    {
      current_arg = arg_start ();
#ifdef CFUNCTIONS_DEBUG
      if (cfunctions_dbug.func)
        printf ("new current_arg for \"%s\" will be created\n", text);
#endif
    }
  arg_add (current_arg, text, yylineno);
  s.function_type_n++;
}

/* Write GNU C extensions for a function. */

void
write_gnu_c_x (void)
{
  /* GNU C chokes on the extensions in inline definitions. */
  
  if (s.c_no_return)
    fprintf (outfile, " X_NO_RETURN" );
  if (s.seen_print_format)
    fprintf (outfile,  " X_PRINT_FORMAT(%d, %d)", 
             pf.value[0], pf.value[1] );
  if (s.no_side_effects)
    {
      if ( s.c_return_value == VOID )
        line_warning ("function with no side effects and void return value");
      fprintf (outfile,  " X_CONST" );
    }
}

/* Print out the current list of function types and the function
   name. */

void 
function_print (void)
{
  int printable;

#ifdef CFUNCTIONS_DEBUG
  if (cfunctions_dbug.func)
    printf ("printing function\n");
#endif 
  if (! s.seen_arguments) 
    return;

  printable = (! s.seen_static || save_static_funcs);

  if (! s.local_func)
    {
      if ( global.file )
        outfile = global.file;
    }
  if (inlining && printable)
    {
      fprintf ( outfile, "\n#ifdef %s\n", inline_macro );
      fprintf ( outfile, "extern inline " );
    }
  if (printable || verbatiming)
    {
      print_comment ();
      print_line_number ();
      cpp_external_print ();
      /* Check for the case PROTO ((void)). */
      if (! n_fargs && ! s.void_arguments)
        check_extensions ();
      arg_fprint (outfile, current_arg);
      arg_tag (current_arg, TAG_FUNCTION);
      argument_print ();
      if (! inlining && ! verbatiming)
        {
          write_gnu_c_x ();
          fprintf (outfile,  ";\n" );
        }
      else /* inlining or verbatiming */
        {
          fprintf (outfile,  "\n{" );

          /* Return from the function here so that `function_reset' is
             not called, because we will need to call `function_print'
             again for the non-inline case. */
          
          if (inlining)
            return;
        }
    }
  function_reset();
}

/* Print the name of the current source file to the standard output. */

void
print_name_stamp (const char * name)
{
  if ( name )
    fprintf (outfile, "/* From `%s': */\n", name);
}

/* Print the name of the current source file into the library (user)
   header file. */

static void
global_name_stamp (const char * file_name, struct outfile * x)
{
  FILE * prev;
  
  prev = outfile;
  if (x->file)
    outfile = x->file;
  print_name_stamp (file_name);
  outfile = prev;
}

/* Print the advert for the top of the header file. */

void
print_advert (void)
{
  if (advert)
    {
      if (! advert_arg)
        fprintf (outfile, advert, VERSION, CFUNCTIONS_URL);
      else
        {
          if (fexists (advert))
            {
              if (fcopy (outfile, advert))
                error ("fcopy failed: %s", strerror(errno));
            }
          else
            warning ("cannot find -a argument `%s'", advert);
        }
    }
}

/* Print a guard wrapper for a header file.  A guard wrapper is the
   `#ifndef' construction to stop the contents of the file being seen
   twice. */

void
wrapper_top (char * h_file_name, char ** h_file_guard)
{
  unsigned i, j = 0, l;

  print_advert();

  fprintf (outfile, "/* This file was generated with:\n`%s' */\n",
           command_line);

  /* print out so that contents of the header are only included once */

  h_file_name = strip_dir ( h_file_name );

  l = strlen (h_file_name);

  * h_file_guard = malloc_or_exit (l + 50);

  /* `CFH_' abbreviates CFUNCTIONS HEADER FILE.  This should prevent
     clashes I hope. */

  j += sprintf ( * h_file_guard, "CFH_" );

  for ( i = 0; i < l + 1; i++ )
    {
      /* If this fouls up on any other weird characters in file names,
         one can just add another case here. */

      switch ( h_file_name[i] )
        {
        case '.':
        case '+':
        case '-':
          j += sprintf ( * h_file_guard + j, "_");
          break;
        default:
          (* h_file_guard)[j++] = toupper (h_file_name[i]);
        }
    }
  i = fprintf (outfile, "#ifndef %s\n#define %s\n\n", 
               * h_file_guard, * h_file_guard );
#ifdef CFUNCTIONS_DEBUG
  if ( ! i )
    bug (HERE, "print failure for wrapper top: outfile not assigned?");
#endif
}

/* Write the bottom wrapper (the `#endif') of the guard wrapper for a
   `.h' file (see the comments before the function `wrapper_top' for
   an explanation).  */

void
wrapper_bottom(char * h_file_guard)
{
  fprintf (outfile, "\n#endif /* %s */\n", h_file_guard );
  free ( h_file_guard );
  h_file_guard = NULL;
}

/* Check the two files for differences.  If they are the same, then
   remove the new one, and rename the backup file to what it was
   before.  */

void
unbackup(char * backup_name, char * file_name)
{
  int i;

#ifdef CFUNCTIONS_DEBUG
  if ( backup_name == file_name ||
       strcmp(backup_name, file_name) == 0 )
    bug ( HERE, "backup name and file name the same" );
#endif

  i = fdiff (backup_name, file_name);
  if (! i)
    {
      if (rename ( backup_name, file_name ))
        error ( "could not rename %s to %s: %s", backup_name,
                file_name, strerror(errno));
    }
  else if ( i < 0 )
    /* Deficiency: My function fdiff just returns an obscure negative
       number on failure. */
    error ( "failure number %d in fdiff", i );
  else if ( ! backup )
    if ( unlink ( backup_name ) )
      error ( "removal of backup %s failed: %s", backup_name, 
              strerror ( errno ) );
  free (backup_name);
}

/* Read an input C file. */

void
read_file (void)
{
  yylineno = 1;

  while (yylex ())
    ;

  function_reset ();

  if (! initial_state ())
    {
      warning ( "was parsing %s in %s when the file unexpectedly ended", 
                state_message (), source_name);
      start_initial ();
    }

  if ( curly_braces_depth )
    {
      warning ("%d too many open braces in `%s'", curly_braces_depth,
               source_name );

      /* Reset this variable for the next source file to prevent
         cascading error messages. */

      curly_braces_depth = 0;
    }
}

/* Copy a header file to a back up. */

char *
do_backup (char * file_name)
{
  char * b;

  b = find_backup_file_name (file_name);
  if (fexists (file_name))
    {
      if (rename (file_name, b))
        error ( "could not rename `%s' to `%s': %s", file_name, b, 
                strerror (errno));
      return b;
    }
  else
    {
      free (b);
      return NULL;
    }
}

/* 
   Extract function names from a C file.
*/

void
extract (char * c_file_name)
{
  char * h_file_name = NULL, * h_file_guard;
  unsigned c_file_name_len;
  char * backup_name = NULL;

  if ( c_file_name )
    {
      tag_new_file ( c_file_name );
      source_name = c_file_name;
      c_file_name_len = strlen (c_file_name);
      
#ifndef DISABLE_CPP
      if (c_preprocess)
        yyin = cpp_start (c_file_name);
      else
#endif
        yyin = fopen_or_exit (c_file_name, "r");

      if ( individual )
        {
          h_file_name = malloc_or_exit (c_file_name_len + 1);
          strcpy ( h_file_name, c_file_name );
          h_file_name[c_file_name_len - 1] = 'h';
          backup_name = do_backup (h_file_name);
          localfile = fopen_or_exit ( h_file_name, "w" );
          outfile = localfile;
          n_local_writes = 0;
          wrapper_top ( h_file_name, & h_file_guard );
          written_c_extensions = 0;
        }
    }
  else
    {
      source_name = "STDIN";
      reading_from_stdin = TRUE;
#ifndef DISABLE_CPP
      if (c_preprocess)
        yyin = cpp_start (NULL);
#endif
    }

  if ( ! individual )
    {
      if ( local.file )
        localfile = local.file;
      else
        {
          localfile = stdout;
          outfile = localfile;
          if (wrap)
            wrapper_top (wrap, & h_file_guard);
        }
    }
  outfile = localfile;
      
  global_name_stamp (c_file_name, & global);

  read_file ();

  if ( individual )
    {
      wrapper_bottom (h_file_guard);
      fclose ( localfile );

      /* check whether anything was written to the `individual' file */

      if ( global.file && ! n_local_writes && ! keep_empty_files )
        {
          if ( backup_name )
            {
              if ( rename (backup_name, h_file_name))
                error ( "could not rename `%s' to `%s': %s", backup_name,
                        h_file_name, strerror(errno));
              free (backup_name);
            }
          else
            unlink (h_file_name);
        }
      else if (backup_name)
        unbackup (backup_name, h_file_name);
    }
  else
    if (wrap)
      wrapper_bottom (h_file_guard);

  if (h_file_name)
    free (h_file_name);
}

/* Start writing a general header which covers more than one input
   file. */

static void
open_library_output ( struct outfile * x )
{
  if ( x->name )
    {
      char * file_aux_name;

      /* Make the `.h' file name. */

      x->name_len = strlen (x->name);
      x->file_name = malloc_or_exit (x->name_len + 3);
      file_aux_name = malloc_or_exit (x->name_len + 5);
      strcpy (x->file_name, x->name);
      strcat (x->file_name, ".h");
      
      /* Back up any old file */
      
      x->backup_name = do_backup (x->file_name);

      /* Write the top wrapper and `.hin' header information */

      x->file = fopen_or_exit (x->file_name, "w");
      outfile = x->file;
      wrapper_top (x->file_name, & x->guard_name);
      strcpy (file_aux_name, x->file_name);
      strcat (file_aux_name, "in");
      if (fexists (file_aux_name))
        {
          verbatiming = TRUE;
          hin_copying = TRUE;
          verbatim_file = x->file;
          source_name = file_aux_name;
          tag_new_file (file_aux_name);
#ifndef DISABLE_CPP
          if (c_preprocess)
            yyin = cpp_start (file_aux_name);
          else
#endif
            yyin = fopen_or_exit (file_aux_name, "r");
          global_name_stamp (file_aux_name, x);
          read_file ();
          #ifdef CFUNCTIONS_DEBUG
          if ( ! verbatiming ) 
            bug (HERE, "verbatiming was turned off while reading %s",
                 file_aux_name);
          #endif
          verbatiming = FALSE;
          hin_copying = FALSE;
        }
      free (file_aux_name);
    }
  outfile = localfile;
}

/* finish off the library (user) header file */

static void
close_library_output (struct outfile * x)
{
  if ( ! x->file )
    return;
  outfile = x->file;
  wrapper_bottom ( x->guard_name );
  fclose ( x->file );
  if ( x->backup_name )
    unbackup ( x->backup_name, x->file_name );
  free ( x->file_name );
}

#ifdef CFUNCTIONS_DEBUG

void
set_debug_flag ( char * flag_name )
{
  if (strcmp(flag_name, "tag")==0)
    tag_debug = 1;
  else if (strcmp(flag_name, "func")==0)
    cfunctions_dbug.func = 1;
  else if (strcmp(flag_name, "cpp")==0)
    cfunctions_dbug.cpp = 1;
  else if (strcmp(flag_name, "comment")==0)
    cfunctions_dbug.comm = 1;
  else if (strcmp(flag_name, "brace")==0)
    cfunctions_dbug.brace = 1;
  else if (strcmp(flag_name, "arg")==0)
    cfunctions_dbug.arg = 1;
  else if (strcmp(flag_name, "fptr")==0)
    cfunctions_dbug.fptr = 1;
  else if (strcmp(flag_name, "flex")==0)
    yy_flex_debug = 1;
  else if (strcmp(flag_name, "trad")==0)
    trad_debug = 1;
  else
    warning ( "unknown debug flag `%s': see the Cfunctions manual for a list "
              "of possible debugging flags", flag_name );
}

#endif /* def CFUNCTIONS_DEBUG */

static void
overwrite_check ( char * c_file_name, struct outfile * x )
{
  if ( x->name_len && 
       strlen(c_file_name) == x->name_len + 2 &&
       strncmp (x->name, c_file_name, x->name_len ) == 0 && 
       individual )
    
    error ( "global header `%s.h' will be overwritten by "
            "individual header of `%s'", x->name, c_file_name);
}

static void
preserve_command_line (int argc, char ** argv)
{
  unsigned i, argv_total_len, running_len;

  argv_total_len = 0;
  for (i = 0; i < argc; i++)
    argv_total_len += strlen (argv[i]) + 4;
  command_line = malloc_or_exit (argv_total_len);
  command_line[0] = '\0';
  running_len = 0;
  for (i = 0; i < argc; i++)
    {
      unsigned len = strlen (argv[i]);
      if (len > error_msg_width)
        {
          strcat (command_line, "\n");
          strcat (command_line, argv[i]);
          strcat (command_line, "\n");
          running_len = 2;
          continue;
        }
      else if (running_len + len > error_msg_width) /* see `error_msg.c' */
        {
          running_len = len + 2;
          strcat (command_line, "\n");
        }
      else
        {
          running_len += len + 1;
        }
      strcat (command_line, argv[i]);
      if (i != argc - 1)
        strcat (command_line, " ");
    }
}

const char * const version_info =
"Cfunctions version "VERSION", Copyright "
"(C) "COPYRIGHT_YEAR" Ben K. Bullock.\n"
"Cfunctions comes with NO WARRANTY, to the extent permitted by law.\n"
"You may redistribute Cfunctions under the terms of the GNU General\n"
"Public Licence.  For more information see the file named COPYING.\n";

int 
main (int argc, char ** argv)
{
  int option_index = 0;
  char * c_file_name;
  char * option_string;
  char * version;
  
  program_name = "cfunctions";
  source_line = & yylineno;
  yy_flex_debug = 0;
#ifdef CFUNCTIONS_DEBUG
  yy_flex_debug = 0;
#endif

  version = getenv ("SIMPLE_BACKUP_SUFFIX");
  if (version)
    simple_backup_suffix = version;
  version = getenv ("VERSION_CONTROL");

  /* Get user defaults values from the run command file. */

  read_rc_file (& rc);

  preserve_command_line (argc, argv);

  if (rc.advert)
    {
      advert_arg = TRUE;
      advert = rc.advert;
      if ( strcmp ( advert, "off" ) == 0 )
        advert = NULL;
    }
  backup                = rc.backup;
  copy_c_ex             = rc.copy_c_ex;
  if (rc.global_macro)
    global_macro          = rc.global_macro;
  c_ex_std_include      = rc.include_c_ex;
  keep_empty_files      = rc.keep;
  if (rc.local_macro)
    local_macro           = rc.local_macro;
  write_comments        = rc.write_comments;
  write_line_numbers    = rc.line_numbers;
  if (rc.proto_macro)
    prototype_macro       = rc.proto_macro;
  if (rc.backup_suffix)
    simple_backup_suffix  = rc.backup_suffix;
  if (rc.warn)
    {
      char * warn = rc.warn;
      char * space;
      while (* warn)
        {
          space = strchr (warn, ' ');
          if (space)
            {
              * space = '\0';
              do space++; while (* space == ' ');
            }
          process_warning (warn);
          if (space)
            warn = space;
          else
            break;
        }
    }

  /* parse the command line options. */

  option_string = short_options (long_options, n_options);

  while (1)
    {
      int c;

      c = getopt_long (argc, argv, option_string, long_options, 
                       & option_index);
      if ( c == -1 )
        break;
      switch ( c )
        {
        case 'a':
          advert_arg = TRUE;
          advert = optarg;
          if ( strcmp ( advert, "off" ) == 0 )
            advert = NULL;
          break;
        case 'b':
          backup = TRUE;
          break;
        case 'c':
          copy_c_ex = TRUE;
          break;
        case 'C':
#ifdef DISABLE_CPP
          warning ("C preprocessor support was disabled at compilation time");
#else
          c_preprocess = TRUE;
          cpp_add_argument (CPP);
#endif
          break;
        case 'D':
#ifdef CFUNCTIONS_DEBUG
          set_debug_flag(optarg);
          break;
#endif
        case 'e':
          warning ( "-e/--emacs-tags option doesn't work yet" );
          break;
        case 'g':
          global.name = optarg;
          if ( is_c_file ( global.name ))
            warning ( "global header name `%s' looks like a C file name", 
                      global.name );
          global.name_len = strlen(global.name);
          break;
        case 'G':
          global_macro = optarg;
          break;
        case 'h':
          print_usage ( n_options, long_options, usage );
          exit (EXIT_SUCCESS);
        case 'i':
          individual = TRUE;
          break;
        case 'I':
          c_ex_std_include = TRUE;
          break;
        case 'k':
          keep_empty_files = TRUE;
          break;
        case 'l':
          local.name = optarg;
          if ( is_c_file ( local.name ))
            warning ( "local header name `%s' looks like a C file name", 
                      local.name );
          local.name_len = strlen(local.name);
          break;
        case 'L':
          local_macro = optarg;
          break;
        case 'm':
          write_comments = TRUE;
          break;
        case 'n':
          write_line_numbers = TRUE;
          break;
        case 'o':
          freopen_stdout (optarg);
          break;
        case 'p':
          prototype_macro = optarg;
          break;
        case 'P':
#ifndef DISABLE_CPP
          if (! c_preprocess)
            warning ("ignoring -P / --cpp-arg %s: for C preprocessing "
                     "use `-C / --cpp'", optarg);
          else
            cpp_add_argument (optarg);
#endif
          break;
        case 's':
          save_static_funcs = TRUE;
          break;
        case 'S':
          simple_backup_suffix = optarg;
          break;
        case 't':
          tag_init();
          break;
        case 'v':
          printf (version_info);
          exit (EXIT_SUCCESS);
        case 'V':
          version = optarg;
          break;
        case 'w':
          wrap = optarg;
          break;
        case 'W':
          process_warning (optarg);
          break;
        case 'x':     
          extensions = FALSE;
          break;
        default:
          print_usage ( n_options, long_options, usage );
          exit (EXIT_FAILURE);
        }
    }

  free (option_string);

  if (backup)
    backup_type = get_version (version);

  /* sanity checks for arguments */

  if ( local.name && individual )
    error ("option `--local' is incompatible with option `--individual'");

  if ( individual || global.name || local.name )
    {
      if (save_static_funcs)
        warning ("static functions will be written to header files");
    }
  else
    writing_to_stdout = TRUE;

  if (( individual || ( local.name && global.name)) && wrap)
    warning ( "`--wrap %s' will be ignored because output files "
              "were specified", wrap );
  if (c_preprocess && (copy_c_ex || global.name || local.name))
    warning ("-c, -x, -g, -l options are useless when C preprocessing");

  if ( copy_c_ex )
    extensions = TRUE;

  if (local.name && global.name && strcmp(local.name, global.name) == 0)
    error ("local name `%s' is the same as the global name.", local.name);

  open_library_output ( & global );

  if (local.name)
    open_library_output ( & local );

  if ( optind == argc )
    {
      /* Cfunctions will read from stdin. */

      if (individual)
        error ( "bad option -i / --individual: "
                "Cfunctions cannot write individual .h files for stdin" );
      extract (NULL);
    }
  else

    /* Everything else on the command line is a C file */

    while ( optind < argc )
      {
        c_file_name = argv[optind++];
        if ( ! is_c_file ( c_file_name ) )
          warning ( "`%s' does not look like a C file", c_file_name );
        overwrite_check ( c_file_name, & global );
        overwrite_check ( c_file_name, & local );

        extract (c_file_name);
      }

  close_library_output (& global);

  if (local.name)
    close_library_output (& local);

  tag_exit ();
  exit (EXIT_SUCCESS);
}
