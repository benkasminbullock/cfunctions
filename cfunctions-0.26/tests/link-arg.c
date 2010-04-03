#include "ok-arg.h"

int funkyfunc (int x)
{
  return x*x;
}

int
main ()
{
  return func(3, funkyfunc);
}
