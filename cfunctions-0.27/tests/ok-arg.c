// tags: func

// Test with weird function arguments.

#include "ok-arg.h"

int func ( int a, int (* junk)(int x))
{
  return junk(a);
}
