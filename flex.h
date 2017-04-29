/* The following declarations are for functions defined in
   "cfunctions.fl" which access the parsing state of the flex
   lexer. */

int argument_state ();
int initial_state ();
void pop_state ();
void push_in_cpp ();
void start_initial ();
const char * state_message ();
void clean_up_flex ();
#ifdef FLEX
#define ARGS cfp, yytext, yyleng
static void start_quote (cfparse_t * cfp, const char * yytext, int yyleng, int state);
static void start_function (cfparse_t * cfp);
static void do_main (cfparse_t * cfp, const char * yytext, int yyleng);

#endif /* def FLEX */
