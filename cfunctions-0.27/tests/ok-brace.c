// tags: dog tellytubby

#include "ok-brace.h"

extern int jubbins;

struct dog
{
  enum { x, y, z} beagle;
  int rotweiler;
};

int tellytubby (int dipsy, int tinkywinky, int lala, int po)
{
  char c = '{';
  char cd = '{';
  char cdd = '{';

  if (po == c && jubbins )
    {
      if (dipsy == cd && tinkywinky == cdd )
        {
          return po;
        }
      else
        {
          return po * dipsy;
        }
    }
  return cdd;
}
