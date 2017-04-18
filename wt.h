#ifndef CFH_WT_H
#define CFH_WT_H
extern FILE * outfile;
extern FILE * localfile;
extern FILE * verbatim_file;
extern char line_source_name[];
extern const char * prototype_macro;
extern const char * inline_macro;
extern unsigned curly_braces_depth;

/* Different kinds of preprocessor conditional statements */

typedef enum {
    CPP_ZAP, /* CPP_ZAP must be equal to zero, because it is used in
		tidying up 'retired' stack elements. */
    CPP_IF, CPP_ELSE, CPP_ELIF, CPP_ENDIF, N_CPP_IF_TYPES
}
Cpp_If_Type;

extern unsigned cpp_prints;
extern unsigned cpp_if_now;
extern struct arg * current_arg;
extern unsigned arg_br_depth;
extern unsigned n_local_writes;
extern unsigned string_debug_on;
extern unsigned rule_line;
extern struct arg * * fargs;
extern unsigned n_fargs;
extern unsigned max_fargs;
struct warning {
    unsigned implicit_int      : 1;
    unsigned reserved_words    : 1;
};
extern struct warning warns;
extern const char * const version_info;

struct pf
{
    unsigned index;
    unsigned value[2];
};

extern struct pf pf;
void brace_open (void);
void brace_close (void);
void do_comment_start (void);
void do_comment_end (void);
void do_comment_print (const char * text, int leng);
void do_PRINT_FORMAT (void);
void do_void_pointer (const char * text);
void do_start_arguments (void);
void do_arguments (void);
void do_function_pointer (const char * text);
void do_function_pointer_argument (const char * text);
void do_word (const char * text, int leng);
void do_typedef (const char * text, int leng);
void do_copy_typedef (const char * text, int leng);
void inline_print (const char * x);
void line_change (const char * text);
void copy_c_extensions (void);
void check_extensions (void);
void cpp_add (char * text, Cpp_If_Type type);
void cpp_eject (unsigned u);
void do_start_cpp (const char * text);
void do_escaped_brace (const char * text);
void do_extern (const char * text, int leng);
void do_NO_RETURN (const char * text);
void do_arguments_close_bracket (const char * text, int leng);
void do_LOCAL (const char * text);
void do_static (const char * text, int leng);
void do_void (const char * text, int leng);
void do_NO_SIDE_FX (const char * text);
void do_arguments_open_bracket (const char * text, int leng);
void do_brace_close (void);
void do_void_arguments (void);
void do_define (const char * text);
void cpp_stack_free (unsigned p);
void cpp_external_tidy (void);
void cpp_external_print (void);
void argument_save (const char * text, unsigned text_length);
int no_prototype (void);
void argument_next (void);
void argument_print (void);
void external_clear (void);
void external_print (const char * semicolon, const char * why);
void forward_print (const char * end);
void function_reset (void);
void function_save (const char * text, unsigned yylength);
void write_gnu_c_x (void);
void function_print (void);
void wrapper_bottom (char * h_file_guard);
void read_file (void);
char * do_backup (char * file_name);
#endif /* CFH_WT_H */
