#ifdef HEADER
typedef struct {
    long fowl;
    long fish;
} piscean;
#endif


#include "ok-commas.h"

piscean * stingray(int), * monkfish(int), * junkfish(int);

static piscean fish = {0, 1}, chips;

piscean * stingray (int x)
{
  return 0;
}

piscean * monkfish (int x)
{
  return & fish;
}

piscean * junkfish (int x)
{
  return & chips;
}

