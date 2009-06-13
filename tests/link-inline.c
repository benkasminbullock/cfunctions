#include <stdio.h>

// gcc_opt: -O -Winline

#include "ok-inline.h"

int main ()
{
  printf ( "%d %d %d\n", rub (3), rub(blubber()), rub(22) );
  return 0;
}
