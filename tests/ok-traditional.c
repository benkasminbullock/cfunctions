// options: -p BLOATO

#define BLOATO(a) ()
#include "ok-traditional.h"

char * jub (q, p, r, t, s, x, y, z)
     int q;
     char p, r, t;
     char * s;
{
  s[0] = p;
  if (q)
    s[1] = r;
  s[2]=t++;
  return s;
}
