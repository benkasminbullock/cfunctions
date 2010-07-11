// options: -g hin-test -i

#include "hin-test.h"

#ifndef HUBAJUBA
#error "Macro was not defined"
#endif

int frog (void * nothing)
{
  if (nothing)
    return jub(99, nothing);
  else
    return jub(88, 0);
}

