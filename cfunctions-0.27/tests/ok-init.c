#include "ok-init.h"

// tags: comma_op_init not_init

// I can't get any comma op which doesn't give a warning.

int comma_op_init = 1, not_init;

#if 0

/* No main, no brain. */

int
main()
{
  return 0;
}
#endif /* 0 */
