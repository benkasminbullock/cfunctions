// tags: blubber rub

#include "ok-inline.h"

NO_SIDE_FX INLINE int blubber ()
{
  static int jub;
  
  /* The next thing is to trip some funky bugs (now fixed).  (1) Funky
     bug with % being zapped due to using fprintf (file, x) instead of
     fprintf(file,"%s",x) (2) Funky bug where } inside a function was
     not output. */

  if (jub > 3)
    {
      jub %= 2;
    }

  return jub++;
}

LOCAL INLINE int rub (int q)
{
  return q;
}

