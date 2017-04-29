/* The following functions are fixes to work around not being able to
   access Flex's state (YY_START) outside this file. */

void
push_in_cpp (void)
{
    parser_push_state (in_cpp);
}

int
initial_state (void)
{
    return YY_START == INITIAL;
}

int
argument_state (void)
{
    return YY_START == arguments;
}

void
start_initial (void)
{
    BEGIN (INITIAL);
}

void
pop_state (void)
{
    parser_pop_state (); 
}

typedef struct
{
    const char * file_name;
    int line_number;
    int line_offset;
}
parse_location;

#define MAX_DEPTH 0x100

parse_location state_starts[MAX_DEPTH];

int state_depth = 0;

void parser_push_state (int new_state)
{
    yy_push_state (new_state);
    state_starts[state_depth].line_number = yylineno;
    state_depth++;
    if (state_depth > MAX_DEPTH) {
        fprintf (stderr, "Too many states encountered.");
        exit (EXIT_FAILURE);
    }
}

void parser_pop_state ()
{
    yy_pop_state ();
    state_depth--;
}

/* Return a string corresponding to the state.  Hack around Flex not
   exporting its macros. */

const char * 
state_message (void)
{
    switch (YY_START) {
    case comment:
	return "C comment string";
    case function:
	return "C function or variable declaration";
    case arguments:
	return "C function's argument list";
    case func_ptr:
	return "pointer to a function";
    case func_ptr_arg:
	return "arguments to pointer to a function";
    case print_format:
	return "PRINT_FORMAT instruction";
    case c_string:
	return "C string";
    case in_cpp:
	return "C preprocessor statement";
    case in_struct:
	return "C struct or union";
    case initialiser:
	return "C initialiser";
    case in_enum:
	return "enum definition";
    case is_enum:
	return "enum declaration";
    case traditional:
	return "traditional C function argument declaration";
    case INITIAL:
	return "initial";
    default:
	return "unknown state (report this message as a bug please)";
    }
}

void
clean_up_flex ()
{
    if (yyin) {
	fclose (yyin);
    }
    yylex_destroy ();
}

static void
start_quote (cfparse_t * cfp, const char * yytext, int yyleng, int state)
{
    inline_print (cfp, yytext, yyleng);
    if (state != comment) {
	if (state != c_string ) {
	    parser_push_state (c_string);
	}
	else {
	    parser_pop_state ();
	}                     
    }
}

static void
start_function (cfparse_t * cfp)
{
    function_print (cfp);
    parser_push_state (function);
    brace_open (cfp);
}

static void
do_main (cfparse_t * cfp, const char * yytext, int yyleng)
{
    count_lines (ARGS);
    /* Never write a prototype for main. */
    function_reset (cfp);
    brace_open (cfp);
    parser_push_state (function);
}

static void
do_equals (cfparse_t * cfp)
{
    if (current_arg) {
	arg_put_name (current_arg);
    }
    parser_push_state (initialiser);
    do_discard_initialiser (cfp);
}

static void
do_semicolon (cfparse_t * cfp)
{
    if (current_arg) {                   
	arg_put_name (current_arg);
	external_print (cfp, ";\n");
    }
}

static void
do_comma (cfparse_t * cfp)
{
    if (current_arg) {                   
	arg_put_name (current_arg);
	current_arg = arg_share (current_arg);
    }			
}

static void
do_print_format_argument (cfparse_t * cfp, const char * yytext,
			  int yyleng, struct pf * pf)
{
    char * end;
    if (pf->index > 1) {
	line_error ("bad print format number %d", pf->index);
    }
    pf->value[pf->index] = strtol (yytext, & end, 10);
    if (end == yytext) {
	line_warning ("number '%s' in PRINT_FORMAT could not be parsed",
		      yytext);
    }
}
