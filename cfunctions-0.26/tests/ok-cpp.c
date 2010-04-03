// tags: Gubbins NUBBINS PUG vv blub has_extra_spaces multiline_comment

/* One of the tags in here was actually a mistake. */

#include <stdio.h>
#include "ok-cpp.h"

#define multiline_comment jub /* This is
                                 a multiline comment
                                 which doesn't foul up @:{};']-=_+)(*&^%$£"!
                                 <><>,./? */

#ifdef JUBINS
#define NUBBINS
#endif
#define PUG(jub) jub jub jub
#define   has_extra_spaces 22

#if CONDITION

int

#elif JUBINS

void

#else

char *

#endif

Gubbins ( int b )
{
#if CONDITION

  return 1;

#elif JUBINS

  return;

#else

  return "This silly function.\n";

#endif
}


#if JUBINS

static void v(int i)
{
  return;
}

#endif

void vv() { 
#if X
return; 
#else
int i = 3*4;
v(i);
#endif
}

#if BLOT
int
blub (
#ifdef ZZ
int a,
#elif defined ( GUBBINS )
void * caramel,
#endif
int bbuzz)
{
#if X
  return 33; 
#else
  int i = 3*4;
#endif
  return -i;
}
#endif

#line 33 "jub.c"

#if 0

/* This following declaration from `cfunctions.fl' used to cause a
   problem where the Flex macro {c_space} would collide with the Flex
   macro {cpp}.  The macro {c_space} has now been changed in order to
   only match one `\n'. */

static void function_save ( 
                           #ifdef CFUNCTIONS_DEBUG
                           unsigned line, 
                           #endif
                           char * yytext, unsigned yylength );

#endif /* 0 */
