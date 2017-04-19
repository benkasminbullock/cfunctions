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
