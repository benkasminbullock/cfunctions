// tags: r

#include "ok-void-star.h"

void * r (int q)
{
  if ( q>0 )
    return (void *) "jubbins";
  return (void *) "boo";
}
