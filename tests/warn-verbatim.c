// warning: discarding initialiser value in verbatim section

#include "ok-verbatim.h"

#if 0

/* This is a check for the bug that resulted from the variable
   `verbatim_limit' not being reset when the stack tidying was carried
   out.  The bug was tripped by `#endif' followed immediately by
   `#ifdef HEADER' so that the tidying on the `#endif' was done after
   the start of the verbatim area, thus a mismatch between
   `verbatim_limit' and the stack position of `#ifdef HEADER'. */

#endif

#ifdef HEADER

typedef int fuga;

#if 0
int jub = 99;
#endif /* 0 */

#endif /* HEADER */

fuga func() { return 3;}
