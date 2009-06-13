#include "ok-init.h"

int
main()
{
  if ( not_init )
    {
      return 3;
    }
  return comma_op_init + not_init;
}

