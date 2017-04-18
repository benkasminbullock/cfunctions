/* Main program of Cfunctions. */

#include <ctype.h>
#include <errno.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#include "argument.h"
#include "backup.h"
#include "config.h"
#include "error-msg.h"
#include "file-name.h"
#include "file.h"
#include "sys-or-exit.h"
#include "wt.h"

/* Declarations of Flex objects. */

extern int yylex ();
extern int yylineno;
extern int yy_flex_debug;
extern FILE * yyin;

/* Options. */

struct option long_options [] = {
    {"debug",           required_argument, NULL, 'D'},
    {0, 0, 0, 0}
};

#define N_OPTIONS sizeof (long_options) / sizeof (struct option) - 1

int n_options = N_OPTIONS;

const char * usage [ N_OPTIONS ] = {
    "Set debugging option \"arg\" (-D help for list)",
};

/* Helper functions to work around Flex. */

int argument_state ();
int initial_state ();
void pop_state ();
void push_in_cpp ();
void start_initial ();
const char * state_message ();

/* Macro for printing debugging statements. */

#define DBMSG(format,msg...)					\
    printf ("%s:%d [%s]: ", __FILE__, __LINE__, __func__);	\
    printf (format, ## msg)

#undef BOOL
#define BOOL int
enum bool { FALSE, TRUE };

/* The whole command line. */

static char * command_line;

/* Maximum number of brackets '(' to expect in function arguments. */

#define MAX_ARG_BR_DEPTH 3

/* Are we in a typedef? */

static int in_typedef;

/* The store of information about print format.  This is for the GNU C
   extension '__attribute__((format(printf,,)))' which tells the
   compiler that the arguments to a function are in 'printf' format
   and asks it to check them for errors.  */

/* Files to write output to.  The file 'outfile' is the usual output
   file.  The file 'localfile' is the file for outputting things which
   are prefixed with 'LOCAL'.  The file 'verbatimfile' is the file to
   write verbatim output to.  This can be either 'localfile' or
   'outfile'. */

FILE * outfile;
FILE * localfile;
FILE * verbatim_file;

/* Name of source file from '#line' directives. */

#define MAX_LINE_NAME 0x100
char line_source_name[MAX_LINE_NAME];

static const char * global_macro = "HEADER"; /* -G */

struct outfile
{
    char * name;
    unsigned name_len;
    char * file_name;
    char * guard_name;
    char * backup_name;
    FILE * file;
}
local;                                  /* -l */

const char *prototype_macro = "PROTO"; /* -p */

/* The wrapper for inline macros for GNU C. */

const char * inline_macro = "X_INLINE";

/* The depth of braces '{' and '}' seen. */

unsigned curly_braces_depth;

/* If 'curly_braces_depth' goes beyond the following arbitrary limit
   Cfunctions gives a warning message.  Mostly this is in case
   Cfunctions itself has gone bananas.  Because most counters in
   Cfunctions are unsigned, overflow errors with large numbers
   probably mean underflows have occured.  */

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

    unsigned print_line; /* For debugging: if printed, when was it printed ? */

}
/* The stack of CPP '#if', '#else', '#elif' and '#endif'
   statements. */

cpp_if_stack[MAX_CPP_IFS],

/* An empty element which is copied to places in 'cpp_if_stack'
   which Cfunctions wants to clean up. */

empty_cpp_if;



/* This keeps track of how many lines have been printed by the CPP
   output part. */

unsigned cpp_prints;

/* The top of the 'cpp_if_stack' stack. */

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
    unsigned print : 1;
}
cfunctions_dbug;

unsigned string_debug_on = 0;

/* Are we writing an inline function?  This is not part of the parsing
   state because it needs to survive through 'function_reset'. */

BOOL inlining;

/* Is Cfunctions copying everything?  (this is set by '#ifdef HEADER'
   statements) */

BOOL verbatiming;

/* Is Cfunctions copying a '.hin' file into the header file? */

static BOOL hin_copying;

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

    unsigned seen_extern;

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

    /* FALSE if this is a user function (in other words a global function)
       in a library consisting of several files */

    unsigned local_func : 1;

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

/* The state information is kept in 's'.  The instance 'z' is kept
   empty and used just for copying to reset the state.  Doing this via
   struct copying makes the code simpler and means that Cfunctions
   never forgets to reset all of the states at the end of parsing a
   function or external variable. */

static parse_state_t s;
static parse_state_t z;

/* Line number of rule matched. */

unsigned rule_line;

/* Shared with "argument.c". */

struct warning warns;

static void
print_line_number (void)
{
    fprintf (outfile, "\n#line %u \"%s\"\n", yylineno, source_name);
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
brace_open (void)
{
    curly_braces_depth++;

    if (cfunctions_dbug.brace) {
	DBMSG ("%s:%u: brace debug: '{' (%d deep) at %s:%u\n",
	       source_name, yylineno, curly_braces_depth, __FILE__,
	       __LINE__);
    }
    check_overflow (curly_braces_depth, MAX_CURLY_DEPTH, "curly braces");
}

/* count a '}'. */

void
brace_close (void)
{
    curly_braces_depth--;

    if (cfunctions_dbug.brace) {
	printf ("%s:%u: brace debug: '}' (%d deep)\n",
		source_name, yylineno, curly_braces_depth);
    }
    check_overflow (curly_braces_depth, MAX_CURLY_DEPTH, "curly braces");
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
    /* 'void *' functions have a return value */
    function_save ("void", 4);
    c = strchr (text, '*');
    function_save (c, strlen (c));
}


void
do_start_arguments (void)
{
    if (cfunctions_dbug.arg) {
        DBMSG ("start arguments\n");
    }
    argument_next ();
    arg_put_name (current_arg);
    s.seen_arguments = TRUE;
}

void
do_arguments (void)
{
    if (cfunctions_dbug.arg) {
        DBMSG ("do arguments\n");
    }
    arg_put_name (current_arg);
    s.seen_arguments = TRUE;
}

void
do_function_pointer (const char * text)
{
    if (! current_arg) {
        bug (HERE, "null pointer for current arg");
    }
    current_arg->is_function_pointer = TRUE;

    if (cfunctions_dbug.fptr) {
        DBMSG ("pointer to function '%s'\n", text);
    }

    inline_print (text);
    current_arg->function_pointer = strdup (text);
}

void
do_function_pointer_argument (const char * text)
{
    inline_print (text);

    if (cfunctions_dbug.fptr) {
        DBMSG ("argument to function pointer '%s'\n", text);
    }

    if (current_arg->function_pointer_arguments) {

        /* Append "text" to the end of
           current_arg->function_pointer_arguments. */

        int new_length;
        char * new;

        new_length = strlen (current_arg->function_pointer_arguments) +
            strlen (text) + 1;
        new = malloc_or_exit (new_length);
        sprintf (new, "%s%s", current_arg->function_pointer_arguments, text);
        free (current_arg->function_pointer_arguments);
        current_arg->function_pointer_arguments = new;
    }
    else {
        current_arg->function_pointer_arguments = strdup (text);
    }
}

void
do_word (const char * text, int leng)
{
    if (s.saw_word) {
	pop_state ();
    }
    function_save (text, leng);
    s.saw_word = TRUE;
}

void
do_typedef (const char * text, int leng)
{
    if (verbatiming) {
	function_save (text, leng);
    }
    else {
	s.seen_typedef = TRUE;
    }
}

void
do_copy_typedef (const char * text, int leng)
{
    if (verbatiming) {
        fprintf (outfile, "%s\n", text);
    }
}

void
inline_print (const char * x)
{

    if (cfunctions_dbug.print) {
        DBMSG ("Printing '%s'.\n", x);
    }

    if (inlining) {
        fprintf (outfile, "%s", x);
    }
    else if (verbatiming) {
        if (! in_typedef) {
            fprintf (verbatim_file, "%s", x);
        }
    }
}

/* Deal with preprocessor '#line' directives. */

void
line_change (const char * text)
{
    char * first_quote;
    unsigned name_length;
    char * line_at;
    first_quote = strchr (text, '\"');
    name_length = strlen (first_quote);

    if (name_length > MAX_LINE_NAME) {
	line_warning ("very long source name '%s': truncating",
		      first_quote);
	strncpy (line_source_name, first_quote + 1,
		 MAX_LINE_NAME);
	line_source_name[MAX_LINE_NAME-1] = '\0';
    }
    else {
	strcpy (line_source_name, first_quote + 1);
	line_source_name[name_length-2] = '\0';
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
    yylineno = atoi (line_at) - 1;
    source_name = line_source_name;
    push_in_cpp ();
}



/* Cfunctions sometimes needs to include 'c-extensions.h' because it
   defines all the information about C extensions. */

void
copy_c_extensions (void)
{
    static BOOL c_ex_searched = FALSE;
    static const char * c_ex_file_name = NULL;

    if (! c_ex_searched) {
	c_ex_searched = TRUE;

	/* This could be replaced by a PATH based search. */

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
	fprintf (outfile, "#line 1 \"%s\"\n", c_ex_file_name);
	if (fcopy (outfile, c_ex_file_name)) {
	    warning ("could not copy '%s': %s", c_ex_file_name,
		     strerror (errno));
	}
    }
}

/* Check whether the user has asked for the C extensions, which are
   turned off by default.  If the user needs them, then write a
   statement #include "c-extensions.h".  The user needs the extensions
   file also if he uses traditional C declarations, because the
   definition of a macro 'PROTO' for the traditional C declarations is
   in there.  */

void
check_extensions (void)
{
    static unsigned written_c_extensions;
    if (written_c_extensions) {
	return;
    }
    copy_c_extensions ();
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

	if (cfunctions_dbug.cpp) {
	    printf ("Looking at depth %d: type is '%s', string is '%s'\n", depth,
		    cpp_if_names[cpp_if_stack[depth].type],
		    cpp_if_stack[depth].text ? cpp_if_stack[depth].text : "empty");
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

/* Debugging function to show 'cpp_if_stack'.  */

static void
show_cpp_if_stack (unsigned i)
{
    DBMSG ("CPP debug: cpp_if_stack[%d] = #%s%s\n", i,
	   cpp_if_names[cpp_if_stack[i].type],
	   cpp_if_stack[i].text);
}

/* Pass through the CPP stack and move everything to fill in holes. */

static void
cpp_fill_holes (void)
{
    unsigned i, j = 0;

    for (i = 0; i < cpp_if_now; i++) {
	if (cpp_if_stack[i].type != CPP_ZAP) {
	    if (i != j) {
		cpp_if_stack[j] = cpp_if_stack[i];
		if (verbatim_limit && i == verbatim_limit) {

		    if (cfunctions_dbug.cpp) {
			DBMSG ("CPP debug: lowering verbatim limit "
			       "from %d to %d\n", i, j);
		    }

		    verbatim_limit = j;
		}
	    }
	    j++;
	}
	else {
	    free (cpp_if_stack [ i ].text);
	}
    }

    /* Reset the top of the stack. */

    for (i = j; i < cpp_if_now; i++) {
	cpp_if_stack[i] = empty_cpp_if;
    }

    cpp_if_now = j;

    if (cfunctions_dbug.cpp) {
	DBMSG ("CPP debug: stack size after tidying %d\n", cpp_if_now);
    }
}

/*
  Find all matching #if and #endif pairs in 'cpp_if_stack' and remove
  them, then move all the remaining entries downwards.
*/

static void
cpp_stack_tidy (void)
{
    int i;

    /* Find matching pairs of '#if' and '#endif' and mark them for
       deletion. */

    for (i = cpp_if_now - 1; i >= 0; i--) {
	if (cpp_if_stack[i].type == CPP_ENDIF) {
	    /* Cfunctions has found an '#endif', and it will now descend
	       the stack looking for the partner '#if'. */

	    unsigned endif_level = 1;
	    int depth;
	    BOOL printed = cpp_if_stack[i].printed;

	    depth = i - 1;

	    if (cfunctions_dbug.cpp) {
		DBMSG ("CPP debug: zapping #endif %s\n",
		       cpp_if_stack[i].text);
		if (printed) {
		    DBMSG ("CPP debug: already printed at %u.\n",
			   cpp_if_stack[i].print_line);
		}
		else {
		    DBMSG ("CPP debug: not printed.\n");
		}
	    }
	    while (1) {
		if (depth < 0) {
		    line_error ("too many '#endif's");
		}

		if (cfunctions_dbug.cpp) {
		    show_cpp_if_stack (depth);
		}

		switch (cpp_if_stack[depth].type) {
		case CPP_IF:
		    endif_level--;
		    if (! endif_level) {
			if (! printed && cpp_if_stack[depth].printed) {
			    cpp_eject (i);
			}
			else if (printed && ! cpp_if_stack[depth].printed) {
			    bug (HERE, "#endif printed but #if not printed");
			}
			else {
			    if (cfunctions_dbug.cpp) {
				if (! cpp_if_stack[depth].printed) {
				    DBMSG ("CPP debug: #if was not printed\n");
				}
			    }
			}
			cpp_if_stack[depth].type = CPP_ZAP;
			if (cfunctions_dbug.cpp) {
			    DBMSG ("CPP debug: zapping '#if %s'\n",
				   cpp_if_stack[depth].text);
			}
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
			if (cfunctions_dbug.cpp) {
			    DBMSG ("CPP debug: zapping #else/#elif \"%s\"\n",
				   cpp_if_stack[depth].text);
			    if (! cpp_if_stack[depth].printed) {
				DBMSG ("CPP debug: not printed\n");
			    }
			}
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
    cpp_fill_holes ();

    /* Set everything to 'external'.  Now Cfunctions knows that the C
       preprocessor stuff currently on the stack is not entangled with
       the currently parsed function or variable. */

    for (i = 0; i < cpp_if_now; i++) {
	if (! cpp_if_stack[i].printed) {
	    cpp_if_stack [i].external = 1;
	}
    }
}

/* Push a new thing onto the preprocessor conditional stack. */

void
cpp_add (char * text, Cpp_If_Type type)
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
    yylineno--;


    if (cfunctions_dbug.cpp) {
	DBMSG ("CPP debug: function \"cpp_add\":"
	       "saving '%s' of type '%s' from line %u\n",
	       text, cpp_if_names[type], yylineno);
    }


    x = strstr (text, cpp_if_names[type]);


    if (! x) {
	bug (HERE, "bad string '%s' in cpp_add: should contain '%s'", text,
	     cpp_if_names[type]);
    }


    x += cpp_if_len[type];
    leng = strlen (x);

    if (leng) {
	cpp_stack_top.text = malloc_or_exit (leng + 1);
	strcpy (cpp_stack_top.text, x);
    }
    else
	cpp_stack_top.text = NULL;
    cpp_stack_top.type = type;
    cpp_stack_top.printed = FALSE;

    /* Check for the verbatim copying conditions. */

    if (type == CPP_IF) {
	if (! verbatiming) {
	    if (strstr (x, global_macro)) {
		verbatiming = TRUE;
		verbatim_file = outfile;
		verbatim_limit = cpp_if_now;
		cpp_stack_top.printed = TRUE;
		print_line_number ();
		goto verbatim_started;
	    }
	}
    }

    if (verbatiming) {

	if (! hin_copying) {
	    if (cfunctions_dbug.cpp) {
		DBMSG ("CPP debug: looking for end of verbatim\n");
		DBMSG ("%d %d\n", cpp_stack_find_if (cpp_if_now),
		       verbatim_limit);
	    }
	}

	if (type == CPP_ENDIF && ! hin_copying &&
	    cpp_stack_find_if (cpp_if_now) == verbatim_limit) {
	    /* Cfunctions has hit the final '#endif' of a verbatim copying
	       area. */
	    if (cfunctions_dbug.cpp) {
		DBMSG ("CPP debug: final '#endif' of a verbatim area\n");
	    }
	    cpp_stack_top.printed = TRUE;
	    cpp_stack_tidy ();
	    verbatiming = FALSE;
	}
	else {
	    outfile = verbatim_file;
	    cpp_eject (cpp_if_now);
	}
    }
    else {
	/* The following string acts as a marker to other routines that they
	   should call 'cpp_eject' with the number printed at the end of the
	   string.

	   Each client routine has to call 'atoi' to get the number and call
	   'cpp_eject'. */

	leng = sprintf (cpp_word, "@CPP%u", cpp_if_now);

	if (initial_state ()) {

	    if (cfunctions_dbug.func) {
		DBMSG ("initial state\n");
	    }


	    function_save (cpp_word, leng);
	}
	else if (argument_state ()) {
	    argument_save (cpp_word, leng);
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

    cpp_if_now++;


    if (cfunctions_dbug.cpp) {
	DBMSG ("CPP debug: if stack now %u deep\n", cpp_if_now);
    }


    /* Deficiency: there is no way to resize the stack */

    if (cpp_if_now > MAX_CPP_IFS) {
	line_error ("too many '#if's: limit is %d", MAX_CPP_IFS);
    }
}

/* Write out the preprocessor statement on the stack at position 'u'.  */

void
cpp_eject (unsigned u)
{

    if (cpp_if_stack[u].printed) {
	unsigned j;
	for (j = 0; j < cpp_if_now; j++) {
	    show_cpp_if_stack (j);
	}
	bug (HERE, "Attempt to eject already printed statement %d '%s'",
	     u, cpp_if_stack[u].text);
    }


    if (cpp_if_stack[u].type == CPP_ZAP) {
	return;
    }

    /* The following one might point 'outfile' to the global file
       wrongly when parsing local files. */

    /* Start a new line. */

    fprintf (outfile, "\n");

    if (cpp_if_stack[u].type == CPP_ENDIF) {
	int matching_if;

	matching_if = cpp_stack_find_if (u);

	/* Cfunctions currently prints almost every conditional so the
	   following is not executed. */

	if (! cpp_if_stack [matching_if].printed) {
	    if (cfunctions_dbug.cpp) {
		DBMSG ("CPP debug: '#endif' but matching '#if' not printed\n");
	    }
	    return;
	}
	/* Deficiency: depending on the previous statement, this sometimes
	   prints too many '\n's.  If the last statement ended with '\n'
	   there will be two consecutive '\n's */

	if (cpp_if_stack[u].text) {
	    fprintf (outfile, "#%s%s\n", cpp_if_names[cpp_if_stack[u].type],
		     cpp_if_stack[u].text);
	}
	else
	    fprintf (outfile, "#%s\n", cpp_if_names[cpp_if_stack[u].type]);
    }
    else if (cpp_if_stack[u].text) {
	fprintf (outfile, "#%s%s\n", cpp_if_names[cpp_if_stack[u].type],
		 cpp_if_stack[u].text);
    }
    else {

	if (cpp_if_stack[u].type == CPP_IF ||
	    cpp_if_stack[u].type == CPP_ELIF) {
	    bug (HERE, "%s with no condition",
		 cpp_if_names [cpp_if_stack[u].type]);
	}

	fprintf (outfile, "#%s\n", cpp_if_names [cpp_if_stack[u].type]);
    }
    cpp_if_stack[u].printed = TRUE;



    /* Print a line number to indicate where a particular statement
       was printed. */

    cpp_if_stack[u].print_line = cpp_prints++;
    if (cfunctions_dbug.cpp) {
	DBMSG ("CPP debug: print line %u\n", cpp_if_stack[u].print_line);
    }

}

void
do_start_cpp (const char * text)
{
    if (initial_state ()) {
        function_reset ();
    }
    push_in_cpp ();
    inline_print (text);
}

void
do_escaped_brace (const char * text)
{
    inline_print (text);

    if (cfunctions_dbug.brace) {
	DBMSG ("%s:%u: matched escaped brace.\n",
	       source_name, yylineno);
    }

}

void
do_extern (const char * text, int leng)
{
    if (verbatiming) {
	function_save (text, leng);
    }
    else
	s.seen_extern = TRUE;
}

void
do_NO_RETURN (const char * text)
{
    check_extensions ();
    s.c_return_value = VOID;
    s.c_no_return = TRUE;
    function_save ("void", 5);
}


void
do_arguments_close_bracket (const char * text, int leng)
{
    arg_br_depth--;
    if (! arg_br_depth) {
	start_initial ();
    }
    else {
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
    if (curly_braces_depth == 0) {
	if (inlining) {
	    FILE * outfile_now = outfile;
	    fprintf (outfile, "}");
	    fprintf (outfile, "\n#else /* not def %s */\n", inline_macro);
	    inlining = FALSE;

	    /* Because Cfunctions was copying an inline function, the
	       first call to 'function_print' did not call
	       'function_reset', so that Cfunctions can call
	       'function_print' again here.

	       Unfortunately this resets 'outfile' to 'localfile'. */

	    function_print ();
	    fprintf (outfile_now, "\n#endif /* def %s */\n", inline_macro);
	}
	else if (verbatiming) {
	    fprintf (verbatim_file, "} ");
	}
	pop_state ();
	if (verbatiming && initial_state ()) {
	    fprintf (verbatim_file, "\n");
	}
    }
    else
	inline_print ("}");
}

/* Handle '(void)' argument declarations. */

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
    if (initial_state ()) {
	function_reset ();
    }
    macro_name = (unsigned char *) strstr (text, "define") + 7;
    while (! (isalnum (macro_name[0]) || macro_name[0] == '_')) {
	macro_name++;
    }
    push_in_cpp ();
    inline_print (text);
}

/* Nullify elements of the stack and release their memory. */

void
cpp_stack_free (unsigned p)
{
    if (cpp_if_stack[p].text) {
	free (cpp_if_stack[p].text);
    }
    cpp_if_stack[p] = empty_cpp_if;
}

/* Zap pairs of #if and #endif where #if is 'external'. */

void
cpp_external_tidy (void)
{
    int i;

    /* Find matching pairs of '#if' and '#endif' and mark them for
       deletion. */

    for (i = cpp_if_now - 1; i >= 0; i--) {
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
			    cpp_eject (i);
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
cpp_external_print (void)
{
    unsigned i;


    if (cfunctions_dbug.cpp) {
	DBMSG ("CPP debug: doing external print\n");
    }


    cpp_external_tidy ();

    for (i = 0; i < cpp_if_now; i++) {

	if (cfunctions_dbug.cpp) {
	    DBMSG ("CPP debug: external print of %u\n", i);
	}


	if (cpp_if_stack[i].external && ! cpp_if_stack[i].printed) {
	    cpp_eject (i);
	}

	else if (cfunctions_dbug.cpp) {
	    DBMSG ("CPP debug: not external so rejected\n");
	}

    }
}

/* Arguments to the most recently seen function. */

struct arg ** fargs;

/* Number of arguments to the most recently seen function. */

unsigned n_fargs;

/* Size of allocated memory for 'fargs'. */

unsigned max_fargs;

/* Free all the memory allocated for the argument names and type
   names, then reset the counter for arguments.  This function is
   called when an argument list has been printed. */

static void
argument_reset (void)
{
    unsigned i;


    if (cfunctions_dbug.arg) {
	DBMSG ("argument reset.\n");
    }

    for (i = 0; i < n_fargs; i++) {
	arg_free (fargs[i]);
    }
    n_fargs = 0;
}

/* Allocate memory to copy text into and copy it, then make a pointer
   to it in the types array */

void
argument_save (const char * text, unsigned text_length)
{

    if (cfunctions_dbug.arg) {
	DBMSG ("%s:%u: saving argument '%s' to slot %d\n",
	       source_name, yylineno, text, n_fargs - 1);
    }
    arg_add (fargs[n_fargs - 1], text, 0);
}

/* Test if the function is an ANSI C style one with the arguments'
   types declared or not. */

int
no_prototype (void)
{
    unsigned i;
    for (i = 0; i < n_fargs; i++) {
	struct arg * a = fargs[i];
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
argument_next (void)
{
    n_fargs++;
    if (n_fargs > max_fargs) {
	if (fargs) {
	    max_fargs *= 2;
	    fargs = (struct arg **)
		realloc_or_exit (fargs, sizeof (struct arg *) * max_fargs);
	}
	else {
	    max_fargs = 4;
	    fargs = malloc_or_exit (sizeof (struct arg *) * max_fargs);
	}
    }
    fargs[n_fargs - 1] = arg_start (cfunctions_dbug.arg);
}

/* Print out the list of arguments that were seen between the most
   recently seen pair of brackets '(' and ')' */

void
argument_print (void)
{
    unsigned i;
    int trad;
    trad = s.is_trad && ! (inlining || verbatiming);


    if (cfunctions_dbug.arg) {
	DBMSG ("printing argument\n");
    }


    fprintf (outfile,  " ");
    if (trad) {
	if (warns.strict_prototypes) {
	    line_warning ("function with no prototype");
	}
	fprintf (outfile,  " %s (", prototype_macro);
    }
    if (n_fargs) {
	fprintf (outfile,  "(");

	for (i = 0; i < n_fargs; i++) {
	    arg_fprint (outfile, fargs[i]);

	    if (i < n_fargs - 1) {
		fprintf (outfile, ", ");
	    }
	}
	if (trad) {
	    fprintf (outfile,  "))");
	}
	else
	    fprintf (outfile, ")");
    }
    else {
	if (s.void_arguments) {
	    fprintf (outfile, "(void)");
	}
	else {
	    if (warns.strict_prototypes) {
		line_warning ("function with no prototype");
	    }
	    /* Cfunctions insists on prototypes. */
	    fprintf (outfile, "PROTO ((void))");
        }
    }

    if (! inlining) {
	argument_reset ();
    }
}

/* Print everything external which has been seen regardless of anything
   else. */

void
external_clear (void)
{
    if (verbatiming) {
	arg_fprint (outfile, current_arg);
	fprintf (outfile, "{");
    }
    else if (! (s.seen_static || s.seen_typedef)) {
	s.unnamed_struct = TRUE;
    }
    if (verbatiming || ! s.seen_static) {
	arg_tagable (current_arg);
    }
    arg_free (current_arg);
    current_arg = arg_start (cfunctions_dbug.arg);
}

/* Print an external variable and reset everything.

   The argument is a semicolon char or a '{'.  The semicolon is usual,
   the curly brace occurs when dealing with a named struct such as
   'struct x { ... } y, z' where Cfunctions needs to print the 'x' as
   well as 'y' and 'z'. */

void
external_print (const char * semicolon, const char * why)
{
    int printable;

    printable = ! s.seen_static;

    if (cfunctions_dbug.func) {
        DBMSG ("external print called with argument '%s' because %s.\n",
               semicolon, why);
    }


    if (inlining) {
        bug (HERE, "an external variable cannot be 'inline'");
    }
    if (verbatiming || (! (s.seen_arguments || s.seen_extern) &&
			! s.seen_typedef &&
			! s.unnamed_struct)) {
        if (verbatiming || printable) {
            print_line_number ();
            cpp_external_print ();
            if (current_arg) {
                arg_fprint_all (outfile, current_arg, ! verbatiming);
                if (verbatiming) {
                    if (n_fargs) {
                        argument_print ();
                    }
                }
            }
            fprintf (outfile, "%s", semicolon);
        }
    }

    function_reset ();
}

/* Print something like 'struct empty;'. */

void
forward_print (const char * end)
{
    if (verbatiming) {
	external_print (end, "incomplete forward declaration of struct");
    }
    else {
	function_reset ();
    }
}

/* Free all the memory used in the most recent parse and reset all the
   switches to their default values ready for a new parse. */

void
function_reset (void)
{

    if (cfunctions_dbug.func) {
        DBMSG ("function reset\n");
    }


    if (current_arg) {
	arg_free (current_arg);
    }
    current_arg = NULL;
    s = z;
    cpp_stack_tidy ();
    if (verbatiming) {
	outfile = verbatim_file;
    }
    else
	outfile = localfile;
    argument_reset ();
    in_typedef = 0;
}

/* Save a function type or name in the current list. */

void
function_save (const char * text, unsigned yylength)
{


    if (cfunctions_dbug.func) {
        DBMSG ("%s:%u: saving function word '%s'\n%s:%u: word appears here\n",
               "cfunctions.fl", rule_line, text, source_name, yylineno);
    }


    if (! current_arg) {
        current_arg = arg_start (cfunctions_dbug.arg);

        if (cfunctions_dbug.func) {
            DBMSG ("new current_arg starting \"%s\" will be created\n", text);
        }

        if (strcmp (text, "typedef") == 0) {
            current_arg->is_typedef = 1;
            in_typedef = 1;

            if (cfunctions_dbug.func) {
                DBMSG ("This is a typedef.\n");
            }

        }
    }
    arg_add (current_arg, text, yylineno);
    s.function_type_n++;
}

/* Write GNU C extensions for a function. */

void
write_gnu_c_x (void)
{
    /* GNU C chokes on the extensions in inline definitions. */

    if (s.c_no_return) {
	fprintf (outfile, " X_NO_RETURN");
    }
    if (s.seen_print_format) {
	fprintf (outfile,  " X_PRINT_FORMAT(%d, %d)",
		 pf.value[0], pf.value[1]);
    }

    if (s.no_side_effects) {
	if (s.c_return_value == VOID) {
	    line_warning ("function with no side effects and void return value");
	}
	fprintf (outfile,  " X_CONST");
    }
}

/* Print out the current list of function types and the function
   name. */

void
function_print (void)
{
    int printable;


    if (cfunctions_dbug.func) {
	DBMSG ("printing function\n");
    }

    if (! s.seen_arguments) {
	return;
    }

    printable = ! s.seen_static;

    if (inlining && printable) {
	fprintf (outfile, "\n#ifdef %s\n", inline_macro);
	fprintf (outfile, "extern inline ");
    }
    if (printable || verbatiming) {
	print_line_number ();
	cpp_external_print ();
	/* Check for the case PROTO ((void)). */
	if (! n_fargs && ! s.void_arguments) {
	    check_extensions ();
	}
	arg_fprint (outfile, current_arg);
	argument_print ();
	if (! inlining && ! verbatiming) {
	    write_gnu_c_x ();
	    fprintf (outfile,  ";\n");
	}
	else {/* inlining or verbatiming */
	    fprintf (outfile,  "\n{");

	    /* Return from the function here so that 'function_reset' is
	       not called, because we will need to call 'function_print'
	       again for the non-inline case. */

	    if (inlining) {
		return;
	    }
	}
    }
    function_reset ();
}

typedef enum {
    wt_status_ok,
    wt_status_memory_failure,
}
wt_status_t;

/* Print a guard wrapper for a header file.  A guard wrapper is the
   '#ifndef' construction to stop the contents of the file being seen
   twice. */

#define BSIZE 0x100

static wt_status_t
wrapper_top (const char * h_file_name, char ** h_file_guard)
{
    unsigned i;
    unsigned j = 0;
    unsigned l;
    int maxlen;

    /* Don't add the date to the file stamp, or the diff will not work
       properly. */

    fprintf (outfile,
	     "/*\nThis file was generated by the following command:\n\n"
	     "%s\n\n*/\n",
	     command_line);

    /* print out the C preprocessor wrapper. */

    h_file_name = strip_dir (h_file_name);

    l = strlen (h_file_name);

    maxlen = l + 50;

    * h_file_guard = malloc_or_exit (maxlen);

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
    i = fprintf (outfile,
		 "#ifndef %s\n"
		 "#define %s\n"
		 "\n",
		 * h_file_guard, * h_file_guard);

    if (! i) {
	bug (HERE, "print failure for wrapper top: outfile not assigned?");
    }

    return wt_status_ok;
}

/* Write the bottom wrapper (the '#endif') of the guard wrapper for a
   '.h' file (see the comments before the function 'wrapper_top' for
   an explanation).  */

void
wrapper_bottom (char * h_file_guard)
{
    fprintf (outfile, "\n#endif /* %s */\n", h_file_guard);
    free (h_file_guard);
    h_file_guard = NULL;
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
    if (i == 0) {
	/* Restore "backup_name" to "file_name", overwriting the newly
	   created file. */
	if (rename (backup_name, file_name)) {
	    error ("could not rename %s to %s: %s", backup_name,
		   file_name, strerror (errno));
	}
    }
    else if (i < 0) {
	/* Deficiency: My function fdiff just returns an obscure negative
	   number on failure. */
	error ("failure number %d in fdiff", i);
    }
    free (backup_name);
}

/* Read an input C file. */

void
read_file (void)
{
    yylineno = 1;

    while (yylex ()) {
	;
    }

    function_reset ();

    if (! initial_state ()) {
	warning ("was parsing %s in %s when the file unexpectedly ended",
		 state_message (), source_name);
	start_initial ();
    }

    if (curly_braces_depth) {
	warning ("%d too many open braces in '%s'", curly_braces_depth,
		 source_name);

	/* Reset this variable for the next source file to prevent
	   cascading error messages. */

	curly_braces_depth = 0;
    }
}

/* If a header file with the name "file_name" already exists, rename
   it to a back up with a suffix like ~n~, where n is a number, and
   return the name of the file it is renamed to. If a header file with
   the name "file_name" does not already exist, return 0. */

char *
do_backup (char * file_name)
{
    char * b;

    b = find_backup_file_name (file_name);
    if (fexists (file_name)) {
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

/* Extract function names from a C file. */

static void
extract (char * c_file_name)
{
    char * h_file_name = NULL;
    char * h_file_guard;
    unsigned c_file_name_len;
    char * backup_name;

    if (! c_file_name) {
	fprintf (stderr, "%s:%d: no file name.\n", __FILE__, __LINE__);
	return;
    }
    source_name = c_file_name;
    c_file_name_len = strlen (c_file_name);

    yyin = fopen_or_exit (c_file_name, "r");

    h_file_name = malloc_or_exit (c_file_name_len + 1);
    strcpy (h_file_name, c_file_name);
    h_file_name[c_file_name_len - 1] = 'h';
    backup_name = do_backup (h_file_name);
    localfile = fopen_or_exit (h_file_name, "w");
    outfile = localfile;
    n_local_writes = 0;
    wrapper_top (h_file_name, & h_file_guard);
    outfile = localfile;

    read_file ();

    wrapper_bottom (h_file_guard);
    fclose (localfile);
    if (backup_name) {
	unbackup (backup_name, h_file_name);
    }
    free (h_file_name);
}

static const struct debug_argument {
    char * option;
    char * explanation;
}
debug_options[] = {
    {
	"tag",
	"tag creation"
    },
    {
	"func",
	"function and function argument"
    },
    {
	"cpp",
	"C preprocessor-like actions"
    },
    {
	"comment",
	"C comments",
    },
    {
	"brace",
	"handling of { and }"
    },
    {
	"arg",
	"function arguments"
    },
    {
	"fptr",
	"function pointers"
    },
    {
	"string",
	"C strings"
    },
    {
	"flex",
	"flex lexer"
    },
    {
	"help",
	"Print these messages"
    }
};

static void
set_debug_flag (char * flag_name)
{
    if (strcmp (flag_name, "func") == 0) {
	cfunctions_dbug.func = 1;
    }
    else if (strcmp (flag_name, "cpp") == 0) {
	cfunctions_dbug.cpp = 1;
    }
    else if (strcmp (flag_name, "comment") == 0) {
	cfunctions_dbug.comm = 1;
    }
    else if (strcmp (flag_name, "brace") == 0) {
	cfunctions_dbug.brace = 1;
    }
    else if (strcmp (flag_name, "arg") == 0) {
	cfunctions_dbug.arg = 1;
    }
    else if (strcmp (flag_name, "fptr") == 0) {
	cfunctions_dbug.fptr = 1;
    }
    else if (strcmp (flag_name, "print") == 0) {
	cfunctions_dbug.print = 1;
    }
    else if (strcmp (flag_name, "string") == 0) {
	string_debug_on = 1;
    }
    else if (strcmp (flag_name, "flex") == 0) {
	yy_flex_debug = 1;
    }
    else if (strcmp (flag_name, "help") == 0) {
	int i;
	printf ("The following flags may be passed after -D:\n");
	for (i = 0; i < sizeof (debug_options) / sizeof (struct debug_argument); i++) {
	    printf ("%10s: debug %-60s\n",
		    debug_options[i].option,
		    debug_options[i].explanation);
	}
	// exit ok
	exit (0);
    }
    else {
	warning ("unknown debug flag '%s': see the Cfunctions manual for a list "
		 "of possible debugging flags", flag_name);
    }
}

int
main (int argc, char ** argv)
{
    int option_index = 0;
    char * c_file_name;
    char * option_string;
    int cllen;
    char * clnext;
    int clused;
    int i;
    cllen = 0;
    for (i = 0; i < argc; i++) {
	cllen += strlen (argv[i]) + 1;
    }
    command_line = calloc_or_exit (sizeof (*(command_line)), cllen);
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
    program_name = "cfunctions";
    source_line = & yylineno;
    yy_flex_debug = 0;

    /* Parse the command line options. */

    option_string = "D";

    while (1) {
	int c;

	c = getopt_long (argc, argv, option_string, long_options,
			 & option_index);
	if (c == -1) {
	    break;
	}
	switch (c) {
	case 'D':
	    set_debug_flag (optarg);
	    break;
	default:
	    return EXIT_FAILURE;
	}
    }

    /* sanity checks for arguments */

    while (optind < argc) {
	c_file_name = argv[optind++];
	if (! is_c_file (c_file_name)) {
	    warning ("'%s' does not look like a C file", c_file_name);
	}
	extract (c_file_name);
    }
    return 0;
}
