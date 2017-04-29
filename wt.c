/* Main program of Cfunctions. */

#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#include "argument.h"
#include "backup.h"
#include "cfunctions.h"
#include "config.h"
#include "error-msg.h"
#include "file.h"
#include "sys-or-exit.h"
#include "wt.h"

#include "flex.h"

static int wt_n_mallocs;

#undef BOOL
#define BOOL int
enum bool { FALSE, TRUE };

/* Maximum number of brackets '(' to expect in function arguments. */

#define MAX_ARG_BR_DEPTH 3

/* Name of source file from '#line' directives. */

static const char * global_macro = "HEADER";

/* If the number of "{" characters, 'cfp->curly_braces_depth', goes
   beyond the following arbitrary limit, Cfunctions gives a warning
   message in case Cfunctions itself has overcounted. */

#define MAX_CURLY_DEPTH 20

/* From here is the CPP stuff. */

#define MAX_CPP_IFS 200

struct cpp_if
{
    Cpp_If_Type type;
    char * text;          /* This can be NULL if not '#if' or '#elif'. */
    unsigned printed : 1; /* TRUE if this has been printed */

    /* Boolean 'external' is TRUE if this CPP element is outside an
       argument or function definition list.  This is set for all
       elements on which a 'tidy' is performed but not for elements
       inside a function for example. */

    unsigned external : 1;

    /* For debugging: if printed, when was it printed ? */

    unsigned print_line;
}

/* The stack of CPP '#if', '#else', '#elif' and '#endif'
   statements. */

cpp_if_stack[MAX_CPP_IFS],

/* An empty element which is copied to places in 'cpp_if_stack'
   which Cfunctions wants to clean up. */

empty_cpp_if;

#define cpp_stack_top cpp_if_stack[cfp->cpp_if_now]

/* The maximum number of chars allowed in a struct name. */

#define MAX_STRUCT_CHARS 0x100

/* The parsing state information.  'function_reset' resets this to all
   zeros. */

typedef struct cfunctions_parse_state
{
    /* The number of function arguments seen so far. */

    unsigned function_type_n;

    /* set to VOID only if the function does not have a return value */

    enum { RETURN, VOID } c_return_value;
    
    /* Have we seen 'extern'?  This is needed to extract global variables
       correctly ('extern' global variables are ignored by Cfunctions). */

    unsigned seen_extern : 1;

    /* Have we seen 'static'? */

    unsigned seen_static : 1;

    /* Have we seen the 'inline' keyword? */

    unsigned seen_inline : 1;

    /* TRUE if function has no side effects. */

    unsigned no_side_effects : 1;

    /* TRUE if function arguments have been seen */

    unsigned seen_arguments : 1;

    /* TRUE if the function never returns (for example 'exit').
       This is set by 'NO_RETURN'. */

    unsigned c_no_return : 1 ;

    /* TRUE if PRINT_FORMAT argument was seen.  */

    unsigned seen_print_format : 1;

    /* TRUE if typedef was seen. */

    unsigned seen_typedef : 1;

    /* TRUE if struct with no name and not static, not in typedef. */

    unsigned unnamed_struct : 1;

    /* TRUE if this function is a traditional C function. */

    unsigned is_trad : 1;

    /* TRUE if Cfunctions saw a word after a 'struct', 'union' or 'enum'
       statement. */

    unsigned saw_word : 1;

    /* TRUE for '(void)' functions. */

    unsigned void_arguments : 1;

    /* True if this is a function pointer. */

    unsigned function_pointer : 1;
}
parse_state_t;

/* The top-level thing which is being parsed. */

struct arg * current_arg;

/* The maximum file name length allowed in #line directives. */

#define MAX_LINE_NAME 0x100

struct cfparse
{
    /* This keeps track of how many lines have been printed by the CPP
       output part. */

    unsigned cpp_prints;

    /* The top of the 'cpp_if_stack' stack. */

    unsigned cpp_if_now;

    /* Is Cfunctions copying everything?  This is set by '#ifdef
       HEADER' statements. */

    BOOL verbatiming;

    /* The current file name for #line directives. */

    char line_source_name[MAX_LINE_NAME];

    /* The depth of braces '{' and '}' seen. */

    unsigned curly_braces_depth;

    /* Are we in a typedef? */

    BOOL in_typedef;

    /* Arguments to the most recently seen function. */

    struct arg ** fargs;

    /* Number of arguments to the most recently seen function. */

    unsigned n_fargs;

    /* Size of allocated memory for 'fargs'. */

    unsigned max_fargs;

    /* The number of brackets inside an argument list.  Brackets inside an
       argument list are most likely to happen for function pointer
       arguments.

       Deficiency: I am not sure where else it could happen.  There is a
       MAX variable just in case things go wrong. */

    int arg_br_depth;

    FILE * outfile;

    char * command_line;

    /* The current line number. This replaces yylineno. */

    unsigned ln;

    /* Parse state information. */

    parse_state_t s;
}
cfparser;

cfparse_t * cfp = & cfparser;

/* 
   Given a file name, strip out all the directory information leaving
   just the file name.  

   Return value: a pointer to somewhere in `file_name'.  If there are
   no directories in `file_name', a pointer to `file_name'.
*/

static const char *
strip_dir (const char * file_name)
{
    const char * last_dir;

    last_dir = strrchr (file_name, '/');

    if (! last_dir) {
        last_dir = file_name;
    }
    else {
        last_dir++;
    }
    return last_dir;
}

/* 
   Check whether a string (file_name) ends in `.c'. 

   Return value: position of `.c' if present
                 NULL if .c not at end.
*/

static const char * 
is_c_file (const char * file_name)
{
    char * s;

    s = strstr (file_name, ".c");
    if (! s) {
        return 0;
    }
    if (s[2] == '\0') {
        return s;
    }
    else {
        return 0;
    }
}

/* Print the line declaration for the current line relative to the
   source file. */

static void
print_line_number (cfparse_t * cfp)
{
    fprintf (cfp->outfile, "\n#line %u \"%s\"\n", cfp->ln,
	     get_source_name ());
}

/* Check whether a particular quantity has overflowed a maximum, and if
   so issue a warning. */

static void
check_overflow (unsigned n, const unsigned max, const char * name)
{
    if (n > max) {
	line_warning ("possible overflow of %s: %u > %u",
		      name, n, max);
    }
}

/* count a '{'. */

void
brace_open (cfparse_t * cfp)
{
    cfp->curly_braces_depth++;

    check_overflow (cfp->curly_braces_depth, MAX_CURLY_DEPTH, "curly braces");
}

/* Count a '}'. */

void
brace_close (cfparse_t * cfp)
{
    cfp->curly_braces_depth--;

    check_overflow (cfp->curly_braces_depth, MAX_CURLY_DEPTH, "curly braces");
}

/* The name do_PRINT_FORMAT is misleading: this is a temporary fix. */

void
do_PRINT_FORMAT (cfparse_t * cfp)
{
    cfp->s.seen_print_format = TRUE;
    start_initial ();
}

/* This is triggered by void * in the initial state. */

void
do_void_pointer (cfparse_t * cfp, const char * text)
{
    char * c;
    /* 'void *' functions have a return value */
    function_save (cfp, "void", 4);
    c = strchr (text, '*');
    function_save (cfp, c, strlen (c));
}

void
do_start_arguments (cfparse_t * cfp)
{
    argument_next (cfp);
    arg_put_name (current_arg);
    cfp->s.seen_arguments = TRUE;
    cfp->arg_br_depth++;
}

void
count_lines (cfparse_t * cfp, const char * yytext, int yyleng)
{
    int i;
    for (i = 0; i < yyleng; i++) {
	if (yytext[i] == '\n') {
	    cfp->ln++;
	}
    }
}

void
do_arguments (cfparse_t * cfp, const char * yytext, int yyleng)
{
    count_lines (cfp, yytext, yyleng);
    arg_put_name (current_arg);
    cfp->s.seen_arguments = TRUE;
}

/* Add function pointers in the argument list. */

void
do_function_pointer (cfparse_t * cfp, const char * text, int yyleng)
{
    if (! current_arg) {
        bug (HERE, "null pointer for current arg");
    }
    current_arg->is_function_pointer = TRUE;

    inline_print (cfp, text, yyleng);
    current_arg->function_pointer = strdup_or_exit (text);
}

/* Add function pointer arguments. */

void
do_function_pointer_argument (cfparse_t * cfp, const char * text, int yyleng)
{
    inline_print (cfp, text, yyleng);

    if (current_arg->function_pointer_arguments) {

        /* Append "text" to the end of
           current_arg->function_pointer_arguments. */

        int new_length;
        char * new;

        new_length = strlen (current_arg->function_pointer_arguments) +
            strlen (text) + 1;
        new = malloc_or_exit (new_length);
	wt_n_mallocs++;
        sprintf (new, "%s%s", current_arg->function_pointer_arguments, text);
        CALLX (free_or_exit (current_arg->function_pointer_arguments));
	wt_n_mallocs--;
	current_arg->function_pointer_arguments = 0;
        current_arg->function_pointer_arguments = new;
    }
    else {
        current_arg->function_pointer_arguments = strdup_or_exit (text);
    }
}

/* Add a word. */

void
do_word (cfparse_t * cfp, const char * text, int leng)
{
    if (cfp->s.saw_word) {
	pop_state ();
    }
    function_save (cfp, text, leng);
    cfp->s.saw_word = TRUE;
}

/* This is triggered by a "typedef" statement. The typedef statement
   means that what follows it is not to be exported into the header
   file unless the user requests that, unlike struct or enum, where we
   need to put the extern declaration of the struct or enum or union
   into the header file. */

void
do_typedef (cfparse_t * cfp, const char * text, int leng)
{
    if (cfp->verbatiming) {
	function_save (cfp, text, leng);
    }
    else {
	cfp->s.seen_typedef = TRUE;
    }
}

/* Copy an entire typedef, up to the final semicolon, if in verbatim
   mode. */

void
do_copy_typedef (cfparse_t * cfp, const char * text, int leng)
{
    if (cfp->verbatiming) {
        fprintf (cfp->outfile, "%s\n", text);
    }
}

/* If we are in verbatim copying mode, print whatever is in "x" to the
   output file, otherwise discard it. */

void
inline_print (cfparse_t * cfp, const char * yytext, int yyleng)
{
    count_lines (cfp, yytext, yyleng);
    if (cfp->verbatiming) {
        if (! cfp->in_typedef) {
            fprintf (cfp->outfile, "%s", yytext);
        }
    }
}

/* Deal with preprocessor '#line' directives. */

void
line_change (cfparse_t * cfp, const char * text, int yyleng)
{
    char * first_quote;
    unsigned name_length;
    char * line_at;
    int line;
    char * end;
    first_quote = strchr (text, '\"');
    if (! first_quote) {
	line_warning ("could not find the file name in line directive %s",
		      text);
	push_in_cpp ();
	return;
    }
    name_length = strlen (first_quote);

    if (name_length > MAX_LINE_NAME) {
	line_warning ("very long source name '%s': truncating",
		      first_quote);
	strncpy (cfp->line_source_name, first_quote + 1,
		 MAX_LINE_NAME);
	cfp->line_source_name[MAX_LINE_NAME - 1] = '\0';
    }
    else {
	strcpy (cfp->line_source_name, first_quote + 1);
	cfp->line_source_name[name_length - 2] = '\0';
    }
    line_at = strstr (text, "line");
    if (! line_at) {
	line_at = strstr (text, "#");
	if (! line_at) {
	    bug (HERE, "peculiar mismatch in strstr: "
		 "no 'line' or '#' in '%s'", text);
	}
	line_at += 1;
    }
    else {
	line_at += 4;
    }
    line = strtol (line_at, & end, 10);
    if (line < 1) {
	line_warning ("Invalid line number %d in directive",
		      line);
    }
    if (end == line_at || line == 0) {
	line_warning ("line number in %s could not be parsed",
		      text);
    }
    else {
	cfp->ln = line - 1;
	yylineno = line - 1;
	set_source_name (cfp->line_source_name);
    }
    push_in_cpp ();
}

/* This copies 'c-extensions.h' into the current header file. */

void
copy_c_extensions (cfparse_t * cfp)
{
    static BOOL c_ex_searched = FALSE;
    static const char * c_ex_file_name = NULL;

    if (! c_ex_searched) {
	c_ex_searched = TRUE;

	if (fexists (C_EXTENSIONS_FILE)) {
	    c_ex_file_name = C_EXTENSIONS_FILE;
	}
	else if (fexists (C_EX_LOCAL)) {
	    c_ex_file_name = C_EX_LOCAL;
	}
	else {
	    warning ("cannot find master copy of '%s'", C_EX_FILE_NAME);
	}
    }

    if (c_ex_file_name) {
	fprintf (cfp->outfile, "#line 1 \"%s\"\n", c_ex_file_name);
	fcopy (cfp->outfile, c_ex_file_name);
    }
}

/* Copy the file "c-extensions.h" into the output header file if
   necessary.  */

void
check_extensions (cfparse_t * cfp)
{
    static unsigned written_c_extensions;
    if (written_c_extensions) {
	return;
    }
    copy_c_extensions (cfp);
    written_c_extensions = 1;
}

/* Names of various preprocessor conditions.  These are used to find
   substrings in a Cpp statement. "ZAP" means mark for destruction. */

static const char * cpp_if_names[N_CPP_IF_TYPES] =
{
    "ZAP", "if", "else", "elif", "endif"
};

/* The string lengths of 'cpp_if_names'. */

static unsigned cpp_if_len[N_CPP_IF_TYPES] =
{
    3, 2, 4, 4, 5
};

/*
  Given a '#endif', find the matching '#if'.

  Return value: stack position of matching '#if'.

  Side effects: If this routine cannot find the matching '#if' it
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

    while (1) {
	if (depth < 0) {
	    line_error ("can't find matching #if");
	}

	switch (cpp_if_stack[depth].type) {
	case CPP_IF:
	    endif_level--;
	    if (! endif_level) {
		return depth;
	    }
	    break;
	case CPP_ENDIF:
	    endif_level++;
	    break;
	case CPP_ELSE:
	case CPP_ELIF:
	case CPP_ZAP:
	    break;
	default:
	    bug (HERE, "bad number %d in switch",
		 cpp_if_stack[depth].type);
	}
	depth--;
    }
}

/* Stack position of an '#if' statement which started a 'verbatim
   copying' routine. */

static unsigned verbatim_limit;

/* Compact the CPP stack such that zapped entries are deleted and the
   remaining ones are made contiguous. */

static void
cpp_fill_holes (cfparse_t * cfp)
{
    unsigned i;
    unsigned j;

    j = 0;

    for (i = 0; i < cfp->cpp_if_now; i++) {
	if (cpp_if_stack[i].type != CPP_ZAP) {
	    if (i != j) {
		cpp_if_stack[j] = cpp_if_stack[i];
		if (verbatim_limit && i == verbatim_limit) {
		    verbatim_limit = j;
		}
	    }
	    j++;
	}
	else {
	    if (cpp_if_stack [i].text) {
		CALLX (free_or_exit (cpp_if_stack[i].text));
		wt_n_mallocs--;
		cpp_if_stack [i].text = 0;
	    }
	}
    }

    /* Reset the top of the stack. */

    for (i = j; i < cfp->cpp_if_now; i++) {
	cpp_if_stack[i] = empty_cpp_if;
	if (cpp_if_stack [i].text) {
	    CALLX (free_or_exit (cpp_if_stack[i].text));
	    wt_n_mallocs--;
	    cpp_if_stack [i].text = 0;
	}
    }
    cfp->cpp_if_now = j;
}

/*
  Find all matching #if and #endif pairs in 'cpp_if_stack' and remove
  them, then move all the remaining entries downwards.
*/

static void
cpp_stack_tidy (cfparse_t * cfp)
{
    int i;

    /* Find matching pairs of '#if' and '#endif' and mark them for
       deletion. */

    for (i = cfp->cpp_if_now - 1; i >= 0; i--) {
	if (cpp_if_stack[i].type == CPP_ENDIF) {
	    /* Cfunctions has found an '#endif', and it will now descend
	       the stack looking for the partner '#if'. */

	    unsigned endif_level = 1;
	    int depth;
	    BOOL printed = cpp_if_stack[i].printed;

	    depth = i - 1;

	    while (1) {
		if (depth < 0) {
		    line_error ("too many '#endif's");
		}

		switch (cpp_if_stack[depth].type) {
		case CPP_IF:
		    endif_level--;
		    if (! endif_level) {
			if (! printed && cpp_if_stack[depth].printed) {
			    cpp_eject (cfp, i);
			}
			else if (printed && ! cpp_if_stack[depth].printed) {
			    bug (HERE, "#endif printed but #if not printed");
			}
			cpp_if_stack[depth].type = CPP_ZAP;
			goto zapped;
		    }
		    break;
		case CPP_ENDIF:
		    endif_level++;
		    break;
		case CPP_ELIF:
		case CPP_ELSE:
		    if (endif_level == 1) {
			cpp_if_stack[depth].type = CPP_ZAP;
		    }
		    break;
		case CPP_ZAP:
		    break;
		default:

		    bug (HERE, "bad number %d at depth %d in switch",
			 cpp_if_stack[depth].type, depth);

		    ;
		}
		depth--;
	    }
	zapped:
	    /* Cfunctions has found a matching '#if' and '#endif' pair. */
	    cpp_if_stack[i].type = CPP_ZAP;
	}
    }
    cpp_fill_holes (cfp);

    /* Set everything to 'external'.  Now Cfunctions knows that the C
       preprocessor stuff currently on the stack is not entangled with
       the currently parsed function or variable. */

    for (i = 0; i < cfp->cpp_if_now; i++) {
	if (! cpp_if_stack[i].printed) {
	    cpp_if_stack [i].external = 1;
	}
    }
}

/* Push a new thing onto the preprocessor conditional stack. */

void
cpp_add (cfparse_t * cfp, char * text, Cpp_If_Type type)
{
    char * x;
    unsigned leng;
    char cpp_word[9];

    /* Fix for Flex bug which causes wrong line numbers.  The problem is
       the '$' in the rule for matching C preprocessor stuff.  These rules
       have a dollar to match end of line.  The problem arises because
       Flex actually counts the '\n' which comes after the dollar when it
       is calculating yylineno, and then it 'rejects' the '\n', then
       because it was rejected it counts it again.  I have sent the full
       description of the bug to Vern Paxson, the author of Flex, who said
       it is on the 'to do' list. */

    cfp->ln--;
    yylineno--;

    x = strstr (text, cpp_if_names[type]);

    if (! x) {
	bug (HERE, "bad string '%s' in cpp_add: should contain '%s'", text,
	     cpp_if_names[type]);
    }

    x += cpp_if_len[type];
    leng = strlen (x);

    if (cpp_stack_top.text) {
	bug (HERE, "Unfreed memory at the top of the CPP stack");
    }
    if (leng) {
	cpp_stack_top.text = malloc_or_exit (leng + 1);
	wt_n_mallocs++;
	strcpy (cpp_stack_top.text, x);
    }
    cpp_stack_top.type = type;
    cpp_stack_top.printed = FALSE;

    /* Check for the verbatim copying conditions. */

    if (type == CPP_IF) {
	if (! cfp->verbatiming) {
	    /* Look for the text HEADER. */
	    if (strstr (x, global_macro)) {
		cfp->verbatiming = TRUE;
		verbatim_limit = cfp->cpp_if_now;
		/* Mark this as "printed" although we don't actually
		   print it. */
		cpp_stack_top.printed = TRUE;
		/* Add a line directive containing the current line
		   number of the C file to the output to show that
		   this is a section copied verbatim from the C
		   file. */
		print_line_number (cfp);
		goto verbatim_started;
	    }
	}
    }

    if (cfp->verbatiming) {
	if (type == CPP_ENDIF &&
	    cpp_stack_find_if (cfp->cpp_if_now) == verbatim_limit) {
	    /* Cfunctions has hit the final '#endif' of a verbatim copying
	       area. */
	    cpp_stack_top.printed = TRUE;
	    cpp_stack_tidy (cfp);
	    cfp->verbatiming = FALSE;
	}
	else {
	    cpp_eject (cfp, cfp->cpp_if_now);
	}
    }
    else {
	/* The following string acts as a marker to other routines that they
	   should call 'cpp_eject' with the number printed at the end of the
	   string.

	   Each client routine has to call 'atoi' to get the number and call
	   'cpp_eject'. */

	leng = sprintf (cpp_word, "@CPP%u", cfp->cpp_if_now);

	if (initial_state ()) {
	    function_save (cfp, cpp_word, leng);
	}
	else if (argument_state ()) {
	    argument_save (cfp, cpp_word, leng);
	}
	else {
	    /* Even if the statement is ignored here, it remains on the
	       stack until the next call to 'cpp_stack_tidy'.  The 'printed'
	       flag in 'cpp_ifstack' ensures that an '#endif' will be
	       printed if a corresponding '#if' has been emitted. */
	    ;
	}
    }

 verbatim_started:

    cfp->cpp_if_now++;

    if (cfp->cpp_if_now > MAX_CPP_IFS) {
	line_error ("too many '#if's: limit is %d", MAX_CPP_IFS);
    }
}

/* Write out the preprocessor statement on the stack at position 'u'.  */

void
cpp_eject (cfparse_t * cfp, unsigned u)
{
    if (cpp_if_stack[u].printed) {
	bug (HERE, "Attempt to eject already printed statement %d '%s'",
	     u, cpp_if_stack[u].text);
    }
    if (cpp_if_stack[u].type == CPP_ZAP) {
	return;
    }
    /* Start a new line. */

    fprintf (cfp->outfile, "\n");

    if (cpp_if_stack[u].type == CPP_ENDIF) {
	int matching_if;

	matching_if = cpp_stack_find_if (u);

	/* Cfunctions currently prints almost every conditional so the
	   following is not executed. */

	if (! cpp_if_stack [matching_if].printed) {
	    return;
	}
	/* Deficiency: depending on the previous statement, this sometimes
	   prints too many '\n's.  If the last statement ended with '\n'
	   there will be two consecutive '\n's */

	if (cpp_if_stack[u].text) {
	    fprintf (cfp->outfile, "#%s%s\n", cpp_if_names[cpp_if_stack[u].type],
		     cpp_if_stack[u].text);
	}
	else {
	    fprintf (cfp->outfile, "#%s\n", cpp_if_names[cpp_if_stack[u].type]);
	}
    }
    else if (cpp_if_stack[u].text) {
	fprintf (cfp->outfile, "#%s%s\n", cpp_if_names[cpp_if_stack[u].type],
		 cpp_if_stack[u].text);
    }
    else {

	if (cpp_if_stack[u].type == CPP_IF ||
	    cpp_if_stack[u].type == CPP_ELIF) {
	    bug (HERE, "%s with no condition",
		 cpp_if_names [cpp_if_stack[u].type]);
	}

	fprintf (cfp->outfile, "#%s\n", cpp_if_names [cpp_if_stack[u].type]);
    }
    cpp_if_stack[u].printed = TRUE;

    /* Print a line number to indicate where a particular statement
       was printed. */

    cpp_if_stack[u].print_line = cfp->cpp_prints;
    cfp->cpp_prints++;
}

/* This is triggered by # at the start of a line. */

void
do_start_cpp (cfparse_t * cfp, const char * text, int yyleng)
{
    if (initial_state ()) {
        function_reset (cfp);
    }
    push_in_cpp ();
    inline_print (cfp, text, yyleng);
}

/* This deals with the unusual case of '{' and '}' in the C program
   text. */

void
do_escaped_brace (cfparse_t * cfp, const char * text, int yyleng)
{
    inline_print (cfp, text, yyleng);
}

/* This is triggered by the word "extern" in the C program text. */

void
do_extern (cfparse_t * cfp, const char * text, int leng)
{
    if (cfp->verbatiming) {
	function_save (cfp, text, leng);
    }
    else {
	cfp->s.seen_extern = TRUE;
    }
}

/* This is triggered by cfunctions' special macro "NO_RETURN". */

void
do_NO_RETURN (cfparse_t * cfp, const char * text)
{
    check_extensions (cfp);
    cfp->s.c_return_value = VOID;
    cfp->s.c_no_return = TRUE;
    function_save (cfp, "void", 5);
}

/* This is triggered by ")" in function arguments. */

void
do_arguments_close_bracket (cfparse_t * cfp, const char * text, int leng)
{
    cfp->arg_br_depth--;
    if (cfp->arg_br_depth == 0) {
	/* This was the last ) in the argument list, so revert to the
	   "initial" state. */
	start_initial ();
    }
    else if (cfp->arg_br_depth < 0) {
	bug (HERE, "underflow %d\n", cfp->arg_br_depth);
    }
    else {
	/* This was not the last ) in the argument list, so we need to
	   save it as part of the arguments. */
	argument_save (cfp, text, leng);
    }
}

/* Triggered by the string "static". */

void
do_static (cfparse_t * cfp, const char * text, int leng)
{
    cfp->s.seen_static = TRUE;
    function_save (cfp, text, leng);
}

/* Triggered by the string "void". */

void
do_void (cfparse_t * cfp, const char * text, int leng)
{
    cfp->s.c_return_value = VOID;
    function_save (cfp, text, leng);
}

/* Respond to a '(' in arguments. */

void
do_arguments_open_bracket (cfparse_t * cfp, const char * text, int leng)
{
    cfp->arg_br_depth++;
    check_overflow (cfp->arg_br_depth, MAX_ARG_BR_DEPTH, "brackets");
    argument_save (cfp, text, leng);
}

void
do_brace_close (cfparse_t * cfp)
{
    brace_close (cfp);
    if (cfp->curly_braces_depth == 0) {
	if (cfp->verbatiming) {
	    fprintf (cfp->outfile, "} ");
	}
	pop_state ();
	if (cfp->verbatiming && initial_state ()) {
	    fprintf (cfp->outfile, "\n");
	}
    }
    else {
	inline_print (cfp, "}", strlen ("}"));
    }
}

/* This function is called by the lexer if the function has the
   argument string '(cfparse_t * cfp)'. */

void
do_void_arguments (cfparse_t * cfp, const char * yytext, int yyleng)
{
    cfp->s.void_arguments = TRUE;
    do_arguments (cfp, yytext, yyleng);
}

/* Respond to seeing a #define macro. */

void
do_define (cfparse_t * cfp, const char * text, int yyleng)
{
    if (initial_state ()) {
	function_reset (cfp);
    }
    push_in_cpp ();
    inline_print (cfp, text, yyleng);
}

/* Nullify elements of the stack and release their memory. */

void
cpp_stack_free (unsigned p)
{
    if (cpp_if_stack[p].text) {
	CALLX (free_or_exit (cpp_if_stack[p].text));
	wt_n_mallocs--;
	cpp_if_stack[p].text = 0;
    }
    cpp_if_stack[p] = empty_cpp_if;
}

/* Zap pairs of #if and #endif where #if is 'external'. */

void
cpp_external_tidy (cfparse_t * cfp)
{
    int i;

    /* Find matching pairs of '#if' and '#endif' and mark them for
       deletion. */

    for (i = cfp->cpp_if_now - 1; i >= 0; i--) {
	if (cpp_if_stack[i].type == CPP_ENDIF) {
	    /* Cfunctions has found an '#endif', and it will now descend
	       the stack looking for the partner '#if'. */

	    unsigned endif_level = 1;
	    int depth;

	    for (depth = i - 1; depth >= 0; depth--) {
		switch (cpp_if_stack[depth].type) {
		case CPP_ENDIF:
		    endif_level++;
		    break;
		case CPP_IF:
		    endif_level--;
		    if (! endif_level && cpp_if_stack[depth].external) {
			/* Cfunctions will delete everything on the
			   stack from this statement onwards to i. */
			unsigned j;
			if (cpp_if_stack[depth].printed) {
			    cpp_eject (cfp, i);
			}
			for (j = depth; j <= i; j++) {
			    cpp_stack_free (j);
			}
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
cpp_external_print (cfparse_t * cfp)
{
    unsigned i;
    cpp_external_tidy (cfp);

    for (i = 0; i < cfp->cpp_if_now; i++) {
	if (cpp_if_stack[i].external && ! cpp_if_stack[i].printed) {
	    cpp_eject (cfp, i);
	}
    }
}

/* Free all the memory allocated for the argument names and type
   names, then reset the counter for arguments.  This function is
   called when an argument list has been printed. */

static void
argument_reset (cfparse_t * cfp)
{
    unsigned i;

    for (i = 0; i < cfp->n_fargs; i++) {
	arg_free (cfp->fargs[i]);
    }
    cfp->n_fargs = 0;
}

/* Allocate memory to copy text into and copy it, then make a pointer
   to it in the types array */

void
argument_save (cfparse_t * cfp, const char * text, unsigned text_length)
{
    arg_add (cfp->fargs[cfp->n_fargs - 1], text);
}

/* Test if the function is an ANSI C style one with the arguments'
   types declared or not. */

int
no_prototype (cfparse_t * cfp)
{
    unsigned i;
    for (i = 0; i < cfp->n_fargs; i++) {
	struct arg * a = cfp->fargs[i];
	if (! a->types->t || a->types->t->prev || a->name || a->pointers) {
	    return 0;
	}
    }
    return 1;
}

/* Go on to a new argument.  The last "type" was the name of an
   argument, so move it into the arg_name array and nullify the "type"
   pointer. */

void
argument_next (cfparse_t * cfp)
{
    cfp->n_fargs++;
    if (cfp->n_fargs > cfp->max_fargs) {
	size_t new_size;
	if (cfp->fargs) {
	    cfp->max_fargs *= 2;
	    new_size = cfp->max_fargs * sizeof (struct arg *);
	    cfp->fargs = realloc_or_exit (cfp->fargs, new_size);
	}
	else {
	    cfp->max_fargs = 4;
	    new_size = cfp->max_fargs * sizeof (struct arg *);
	    cfp->fargs = malloc_or_exit (new_size);
	    wt_n_mallocs++;
	}
    }
    cfp->fargs[cfp->n_fargs - 1] = arg_start ();
}

/* Print out the list of arguments that were seen between the most
   recently seen pair of brackets '(' and ')' */

static void
argument_print (cfparse_t * cfp)
{
    unsigned i;
    fprintf (cfp->outfile,  " ");
    if (cfp->n_fargs) {
	fprintf (cfp->outfile,  "(");
	for (i = 0; i < cfp->n_fargs; i++) {
	    arg_fprint (cfp->outfile, cfp->fargs[i]);

	    if (i < cfp->n_fargs - 1) {
		fprintf (cfp->outfile, ", ");
	    }
	}
	fprintf (cfp->outfile, ")");
    }
    else {
	fprintf (cfp->outfile, "()");
    }

    argument_reset (cfp);
}

/* Print everything external which has been seen regardless of anything
   else. */

void
external_clear (cfparse_t * cfp)
{
    if (cfp->verbatiming) {
	arg_fprint (cfp->outfile, current_arg);
	fprintf (cfp->outfile, "{");
    }
    else if (! (cfp->s.seen_static || cfp->s.seen_typedef)) {
	cfp->s.unnamed_struct = TRUE;
    }
    arg_free (current_arg);
    current_arg = arg_start ();
}

/* Print an external variable and reset everything.

   The argument is a semicolon char or a '{'.  The semicolon is usual,
   the curly brace occurs when dealing with a named struct such as
   'struct x { ... } y, z' where Cfunctions needs to print the 'x' as
   well as 'y' and 'z'. */

void
external_print (cfparse_t * cfp, const char * semicolon)
{
    int printable;

    printable = ! cfp->s.seen_static;

    if (cfp->verbatiming || (! (cfp->s.seen_arguments || cfp->s.seen_extern) &&
			! cfp->s.seen_typedef &&
			! cfp->s.unnamed_struct)) {
        if (cfp->verbatiming || printable) {
            print_line_number (cfp);
            cpp_external_print (cfp);
            if (current_arg) {
                arg_fprint_all (cfp->outfile, current_arg, ! cfp->verbatiming);
                if (cfp->verbatiming) {
                    if (cfp->n_fargs) {
                        argument_print (cfp);
                    }
                }
            }
            fprintf (cfp->outfile, "%s", semicolon);
        }
    }

    function_reset (cfp);
}

/* Print something like 'struct empty;'. */

void
forward_print (cfparse_t * cfp, const char * end)
{
    if (cfp->verbatiming) {
	/* Incomplete forward declaration of struct. */
	external_print (cfp, end);
    }
    else {
	function_reset (cfp);
    }
}

/* Free all the memory used in the most recent parse and reset all the
   switches to their default values ready for a new parse. */

void
function_reset (cfparse_t * cfp)
{
    if (current_arg) {
	arg_free (current_arg);
    }
    current_arg = NULL;
    memset (& cfp->s, 0, sizeof (cfp->s));
    cpp_stack_tidy (cfp);
    argument_reset (cfp);
    cfp->in_typedef = 0;
}

/* Save a function type or name in the current list. */

void
function_save (cfparse_t * cfp, const char * text, unsigned yylength)
{
    if (! current_arg) {
        current_arg = arg_start ();
        if (strcmp (text, "typedef") == 0) {
            current_arg->is_typedef = 1;
            cfp->in_typedef = 1;
        }
    }
    arg_add (current_arg, text);
    cfp->s.function_type_n++;
}

/* Write GNU C extensions for a function. */

void
write_gnu_c_x (cfparse_t * cfp)
{
    /* GNU C chokes on the extensions in inline definitions. */

    if (cfp->s.c_no_return) {
	fprintf (cfp->outfile, " X_NO_RETURN");
    }
    if (cfp->s.seen_print_format) {
	fprintf (cfp->outfile,  " X_PRINT_FORMAT(%d, %d)",
		 pf.value[0], pf.value[1]);
    }

    if (cfp->s.no_side_effects) {
	if (cfp->s.c_return_value == VOID) {
	    line_warning ("function with no side effects and void return value");
	}
	fprintf (cfp->outfile,  " X_CONST");
    }
}

/* Print out the current list of function types and the function
   name. */

void
function_print (cfparse_t * cfp)
{
    int printable;
    if (! cfp->s.seen_arguments) {
	return;
    }

    printable = ! cfp->s.seen_static;

    if (printable || cfp->verbatiming) {
	print_line_number (cfp);
	cpp_external_print (cfp);
	arg_fprint (cfp->outfile, current_arg);
	argument_print (cfp);
	if (cfp->verbatiming) {
	    fprintf (cfp->outfile,  "\n{");
	}
	else {
	    write_gnu_c_x (cfp);
	    fprintf (cfp->outfile,  ";\n");
	}
    }
    function_reset (cfp);
}

typedef enum
{
    wt_status_ok,
    wt_status_memory_failure,
}
wt_status_t;

/* Print the top descriptive comment explaining that the file was
   generated automatically and a guard wrapper for a header file.  A
   guard wrapper is the '#ifndef' construction to stop the contents of
   the file being seen twice. */

static wt_status_t
wrapper_top (cfparse_t * cfp, const char * h_file_name, char ** h_file_guard)
{
    unsigned i;
    unsigned j;
    unsigned l;
    int maxlen;

    /* Write a comment explaining how the header file was
       created. This does not write the date in the file stamp, so
       that the diff done by "unbackup" works correctly. That prevents
       header files from being unnecessarily updated, triggering
       unnecessary recompilations. */

    fprintf (cfp->outfile,
	     "/*\nThis file was generated by the following command:\n\n"
	     "%s\n\n*/\n",
	     cfp->command_line);

    /* Print out the C preprocessor wrapper. */

    h_file_name = strip_dir (h_file_name);

    l = strlen (h_file_name);

    maxlen = l + 50;

    * h_file_guard = malloc_or_exit (maxlen);
    wt_n_mallocs++;
    j = 0;

    /* 'CFH_' abbreviates CFUNCTIONS HEADER FILE.  This should prevent
       clashes I hope. */

    j += snprintf (* h_file_guard, maxlen, "CFH_");

    for (i = 0; i < l + 1; i++) {
	/* If this fouls up on any other weird characters in file names,
	   one can just add another case here. */

	switch (h_file_name[i]) {
	case '.':
	case '+':
	case '-':
	    j += snprintf (* h_file_guard + j, maxlen - j, "_");
	    break;
	default:
	    (* h_file_guard)[j] = toupper (h_file_name[i]);
	    j++;
	}
    }
    i = fprintf (cfp->outfile,
		 "#ifndef %s\n"
		 "#define %s\n"
		 "\n",
		 * h_file_guard, * h_file_guard);

    if (! i) {
	bug (HERE, "print failure for wrapper top: cfp->outfile not assigned?");
    }

    return wt_status_ok;
}

/* Write the bottom wrapper (the '#endif') of the guard wrapper for a
   '.h' file (see the comments before the function 'wrapper_top' for
   an explanation).  */

static void
wrapper_bottom (cfparse_t * cfp, char * h_file_guard)
{
    fprintf (cfp->outfile, "\n#endif /* %s */\n", h_file_guard);
    CALLX (free_or_exit (h_file_guard));
    wt_n_mallocs--;
    h_file_guard = 0;
}

/* Check the two files for differences.  If they are the same, then
   remove the new one, and rename the backup file to what it was
   before.  */

static void
unbackup (char * backup_name, char * file_name)
{
    int i;
    if (! backup_name) {
	bug (HERE, "Null pointer backup_name means there is no backup");
    }
    if (strcmp (backup_name, file_name) == 0) {
	bug (HERE, "backup name and file name the same");
    }
    i = fdiff (backup_name, file_name);
    if (i == A_B_SAME) {
	/* Restore "backup_name" to "file_name", overwriting the newly
	   created file. */
	if (rename (backup_name, file_name)) {
	    error ("could not rename %s to %s: %s", backup_name,
		   file_name, strerror (errno));
	}
    }
    else if (i < 0) {
	if (i == NO_B_FILE) {
	    warning ("Output file '%s' was deleted by another process",
		     file_name);
	}
	else if (i == NO_A_FILE) {
	    warning ("Backup file '%s' was deleted by another process",
		     backup_name);
	}
	else {
	    error ("failure number %d in fdiff", i);
	}
    }
    free_backup_name (backup_name);
    backup_name = 0;
}

/* Read an input C file. */

static void
read_file (cfparse_t * cfp)
{
    yylineno = 1;

    while (yylex ()) {
	;
    }

    function_reset (cfp);

    if (! initial_state (cfp)) {
	warning ("was parsing %s in %s when the file unexpectedly ended",
		 state_message (cfp), get_source_name ());
	start_initial (cfp);
    }

    if (cfp->curly_braces_depth > 0) {
	warning ("%d too many open braces in '%s'", cfp->curly_braces_depth,
		 get_source_name ());

	/* Reset this variable for the next source file to prevent
	   cascading error messages. */

	cfp->curly_braces_depth = 0;
    }
}

/* If a header file with the name "file_name" already exists, rename
   it to a back up with a suffix like ~n~, where n is a number, and
   return the name of the file it is renamed to. If a header file with
   the name "file_name" does not already exist, return 0. */

static char *
do_backup (char * file_name)
{

    if (fexists (file_name)) {
	char * b;
	b = find_backup_file_name (file_name);
	if (rename (file_name, b)) {
	    error ("could not rename '%s' to '%s': %s", file_name, b,
		   strerror (errno));
	}
	return b;
    }
    else {
	return 0;
    }
}

/* Given "c_file_name", the name of a C file, make an appropriate
   header name for it. This works by copying the string and then
   replacing the final character with "h". */

static char *
make_h_file_name (const char * c_file_name)
{
    /* The length of "c_file_name". */
    unsigned c_file_name_len;
    /* Return value. */
    char * h_file_name;
    c_file_name_len = strlen (c_file_name);
    h_file_name = malloc_or_exit (c_file_name_len + 1);
    wt_n_mallocs++;
    memcpy (h_file_name, c_file_name, c_file_name_len + 1);
    h_file_name[c_file_name_len - 1] = 'h';
    return h_file_name;
}


/* Extract the function and global variable names from a C file
   specified by "c_file_name" and write them to an equivalent header
   file. */

static void
extract (cfparse_t * cfp, char * c_file_name)
{
    /* The name of the header file, derived from "c_file_name". */
    char * h_file_name;
    /* The ifdef protective wrapper macro name. */
    char * h_file_guard;
    /* The name of a backup file, if it exists. */
    char * backup_name;

    if (! c_file_name) {
	bug (HERE, "no file name");
    }
    set_source_name (c_file_name);
    yyin = fopen_or_exit (c_file_name, "r");
    h_file_name = make_h_file_name (c_file_name);
    backup_name = do_backup (h_file_name);
    cfp->outfile = fopen_or_exit (h_file_name, "w");
    wrapper_top (cfp, h_file_name, & h_file_guard);
    read_file (cfp);
    wrapper_bottom (cfp, h_file_guard);
    fclose_or_exit (cfp->outfile);
    cfp->outfile = 0;
    if (yyin) {
	fclose_or_exit (yyin);
	yyin = 0;
    }
    chmod_or_exit (h_file_name, 0444);
    if (backup_name) {
	unbackup (backup_name, h_file_name);
	backup_name = 0;
    }
    CALLX (free_or_exit (h_file_name));
    wt_n_mallocs--;
    h_file_name = 0;
}

/* This takes the command line in "argc" and "argv" and puts it into
   an allocated string "cfp->command_line" which is used when printing
   out the header file's topmost comment. The command line string is
   freed at the end of main, after the headers have been printed. */

static void
fill_command_line (cfparse_t * cfp, int argc, char ** argv)
{
    int cllen;
    char * clnext;
    int clused;
    int i;
    char * command_line;
    cllen = 0;
    for (i = 0; i < argc; i++) {
	cllen += strlen (argv[i]) + 1;
    }
    command_line = calloc_or_exit (sizeof (*(command_line)), cllen);
    wt_n_mallocs++;
    clnext = command_line;
    clused = 0;
    for (i = 0; i < argc; i++) {
	char * space = "";
	int bytes;
	if (i > 0) {
	    space = " ";
	}
	bytes = snprintf (command_line + clused, cllen - clused, "%s%s",
			  space, argv[i]);
	clused += bytes;
    }
    cfp->command_line = command_line;
}

static void
release_memory (cfparse_t * cfp)
{
    int j;
    for (j = 0; j < MAX_CPP_IFS; j++) {
	cpp_stack_free (j);
    }
    if (cfp->fargs) {
	CALLX (free_or_exit (cfp->fargs));
	wt_n_mallocs--;
	cfp->fargs = 0;
    }
    CALLX (free_or_exit (cfp->command_line));
    wt_n_mallocs--;
    cfp->command_line = 0;
    clean_up_flex ();
    /* Check for memory leaks in other files. */
    arg_memory_check ();
    memory_check ();
    backup_memory_check ();
    file_memory_check ();
    /* Check for memory leaks within this file. */
    if (wt_n_mallocs != 0) {
	fprintf (stderr, "%s:%d: wt_n_mallocs = %d\n", HERE, wt_n_mallocs);
    }
}

int
main (int argc, char ** argv)
{
    char * c_file_name;
    int i;
    fill_command_line (cfp, argc, argv);
    set_program_name ("cfunctions");
    source_line = & yylineno;

    for (i = 1; i < argc; i++) {
	c_file_name = argv[i];
	if (! is_c_file (c_file_name)) {
	    warning ("'%s' does not look like a C file", c_file_name);
	}
	extract (cfp, c_file_name);
    }
    release_memory (cfp);
    return 0;
}

void
do_discard_initialiser (cfparse_t * cfp)
{
    if (cfp->verbatiming) {
	line_warning ("discarding initialiser value in verbatim section");
    }
}
