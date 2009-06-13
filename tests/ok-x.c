// tags: bye_bye jubbins print_msg hubajub

#include "ok-x.h"
#include <stdlib.h>

// Test Cfunctions's C extensions support.

NO_RETURN bye_bye ()
{
  exit ( 1 );
}

NO_SIDE_FX int jubbins ()
{
  return 33;
}

#ifdef HEADER
#include "stdio.h"
#endif

void PRINT_FORMAT ( 1, 2 ) print_msg ( char * msg, ... )
{
  printf ( msg );
}

INLINE void hubajub ()
{
  return;
}
