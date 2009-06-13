#include "ok-struct.h"

int
main ()
{
  fuggy.x = -1;
  muggy = & fuggy;
  fruity = & muggy;
  rubber[19].y = 333;
  return 0;
}
