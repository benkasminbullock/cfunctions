#include <stddef.h>
#include <stdlib.h>
#include <string.h>


/* Duplicate S, returning an identical malloc'd string.  */
char *
strdup (const char *s)
{
  size_t len = strlen (s) + 1;
  void *new = malloc (len);

  if (new == NULL)
    return NULL;

  return (char *) memcpy (new, s, len);
}
