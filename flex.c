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
    fclose (yyin);
    yylex_destroy ();
}
