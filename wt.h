#ifndef CFH_WT_H
#define CFH_WT_H

/* Different kinds of preprocessor conditional statements */

typedef enum {
    CPP_ZAP, /* CPP_ZAP must be equal to zero, because it is used in
		tidying up 'retired' stack elements. */
    CPP_IF, CPP_ELSE, CPP_ELIF, CPP_ENDIF, N_CPP_IF_TYPES
}
Cpp_If_Type;

extern struct arg * current_arg;

/* The store of information about print format.  This is for the GNU C
   extension '__attribute__((format(printf,,)))' which tells the
   compiler that the arguments to a function are in 'printf' format
   and asks it to check them for errors.  */

struct pf
{
    unsigned index;
    unsigned value[2];
};

extern struct pf pf;

typedef struct cfparse cfparse_t;

extern cfparse_t * cfp;

void brace_open (cfparse_t *);
void brace_close (cfparse_t *);
void do_comment_start (cfparse_t *);
void do_comment_end (cfparse_t *);
void do_comment_print (cfparse_t * cfp, const char * yytext, int yyleng);
void do_PRINT_FORMAT (cfparse_t *);
void do_void_pointer (cfparse_t * cfp, const char * yytext, int yyleng);
void do_start_arguments (cfparse_t *);
void do_arguments (cfparse_t * cfp, const char * yytext, int yyleng);
void do_function_pointer (cfparse_t * cfp, const char * yytext, int yyleng);
void do_function_pointer_argument (cfparse_t * cfp, const char * yytext, int yyleng);
void do_word (cfparse_t * cfp, const char * yytext, int yyleng);
void do_typedef (cfparse_t * cfp, const char * yytext, int yyleng);
void do_copy_typedef (cfparse_t * cfp, const char * yytext, int yyleng);
void inline_print (cfparse_t * cfp, const char * yytext, int yyleng);
void line_change (cfparse_t * cfp, const char * yytext, int yyleng);
void copy_c_extensions (cfparse_t *);
void check_extensions (cfparse_t *);
void cpp_add (cfparse_t * cfp, char * text, Cpp_If_Type type);
void cpp_eject (cfparse_t * cfp, unsigned u);
void do_start_cpp (cfparse_t * cfp, const char * yytext, int yyleng);
void do_escaped_brace (cfparse_t * cfp, const char * yytext, int yyleng);
void do_extern (cfparse_t * cfp, const char * yytext, int yyleng);
void do_NO_RETURN (cfparse_t * cfp, const char * yytext, int yyleng);
void do_arguments_close_bracket (cfparse_t * cfp, const char * yytext, int yyleng);
void do_LOCAL (cfparse_t * cfp, const char * yytext);
void do_static (cfparse_t * cfp, const char * yytext, int yyleng);
void do_void (cfparse_t * cfp, const char * yytext, int yyleng);
void do_arguments_open_bracket (cfparse_t * cfp, const char * yytext, int yyleng);
void do_brace_close (cfparse_t *);
void do_void_arguments (cfparse_t * cfp, const char * yytext, int yyleng);
void do_define (cfparse_t * cfp, const char * yytext, int yyleng);
void cpp_stack_free (unsigned p);
void cpp_external_tidy (cfparse_t *);
void cpp_external_print (cfparse_t *);
void argument_save (cfparse_t * cfp, const char * yytext, unsigned yyleng);
void argument_next (cfparse_t *);
void external_clear (cfparse_t *);
void external_print (cfparse_t * cfp, const char * semicolon);
void forward_print (cfparse_t * cfp, const char * end);
void function_reset (cfparse_t *);
void function_save (cfparse_t * cfp, const char * yytext, unsigned yyleng);
void write_gnu_c_x (cfparse_t *);
void function_print (cfparse_t *);
void do_discard_initialiser (cfparse_t * cfp);
void count_lines (cfparse_t * cfp, const char * yytext, int yyleng);
void do_array_arguments (cfparse_t * cfp, const char * yytext, unsigned yyleng);
#endif /* CFH_WT_H */
