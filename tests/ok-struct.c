// tags: bog jub fuggy muggy fruity rubber rub adub dub

#include "ok-struct.h"

// Test non-failure cases of struct matching.

#ifdef HEADER

struct bog
{
  int x, y;
};

struct {
  int a, b;
} jub;

#endif /* HEADER */

struct bog fuggy, * muggy, ** fruity, rubber[20];

typedef struct { int p; int q; } rub, * adub, ** dub[33];
