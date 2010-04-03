// tags: the eenum with no name jubbins e f g h i j k enum_func NO NAME a b c d

// Test code for `enum' statements

#include "ok-enum.h"

#ifdef HEADER
enum { the, eenum, with, no, name } jubbins;
enum a { b, c, d};
#endif

enum a e, f[2], g = d, * h, ** i, j = b, k[2] = {b, c};

enum a enum_func ( enum a x )
{
  return b;
}

enum { NO, NAME };
