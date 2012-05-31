/* Convert names and return them in allocated memory. */

#include <ctype.h>
#include <string.h>
#include "sys_or_exit.h"
#include "name-convert.h"

/* Given a "StringLikeThis", return "slt" in allocated memory. */

char * 
caps_to_initials (const char * caps)
{
  const char * c;
  char * initials, * d;
  unsigned n_caps = 0;
  for (c = caps; * c; c++)
    if (isupper (* c))
      n_caps++;
  d = initials = malloc_or_exit (n_caps + 1);
  for (c = caps; * c; c++)
    if (isupper (* c))
      *d++ = tolower (* c);
  * d = '\0';
  return initials;
}

/* Given a "string_like_this", return a "string like this" in
   allocated memory. */

char *
uscores_to_spaces (const char * uscores)
{
  char * c;
  char * spaces = strdup (uscores);
  for (c = spaces; * c; c++)
    if (*c == '_')
      *c = ' ';
  return spaces;
}

/* Given a "  String liKe this ", return a "string_like_this" in
   allocated memory. */

char *
spaces_to_uscores (const char * spaces)
{
  const char * c;
  char * d;
  char * uscores;

  /* Skip leading spaces. */

  while (isspace (*spaces))
    spaces++;

  uscores = malloc_or_exit (strlen (spaces) + 1);
  for (c = spaces, d = uscores; * c;)
    {
      if (isspace (* c))
        {
          * d++ = '_';
          while (isspace (* c))
            {
              c++;
              /* Remove any trailing underscore from the end of
                 uscores if the string's end is reached. */
              if (! * c)
                * --d = '\0';
            }
        }
      else
        * d++ = tolower (* c++);
    }
  * d = '\0';
  return uscores;
}

/* Given a "  String liKe this ", return a "string-like-this" in
   allocated memory. */

char *
spaces_to_hyphens (const char * spaces)
{
  char * c;
  char * hyphens = spaces_to_uscores (spaces);
  for (c = hyphens; * c; c++)
    if (* c == '_')
      * c = '-';
  return hyphens;
}

/* Given a StringLikeThis, return a string_like_this in allocated
   memory. 

   Bug: this assumes that _ is not the last character of `caps'. */

char * 
caps_to_uscores (const char * caps)
{
  const char * c;
  char * d, * r;
  unsigned len = strlen (caps);

  r = malloc_or_exit (2 * len);
  r[0] = tolower (caps[0]);
  for (c = caps + 1, d = r + 1; * c; c++)
    {
      if (isupper (* c))
        {
          * d++ = '_';
          * d++ = tolower (* c);
        }
      else
        * d++ = * c;
    }
  * d = '\0';
  return r;
}

/* Given a string_like_this, return a StringLikeThis in allocated
   memory. */

char * 
uscores_to_caps (const char * uscores)
{
  const char * c;
  char * d, * r;
  unsigned len = strlen (uscores);

  r = malloc_or_exit (len + 1);
  r[0] = toupper (uscores[0]);
  for (c = uscores + 1, d = r + 1; * c; c++)
    {
      if (* c == '_')
        * d++ = toupper (* ++c);
      else
        * d++ = * c;
    }
  * d = '\0';
  return r;
}

/* Return the upper-case version of a lower case string. */

char *
lower_to_upper (const char * lower)
{
  char * r;
  unsigned len = strlen (lower), i;

  r = malloc_or_exit (len + 1);
  for (i = 0; i < len; i++)
    r[i] = toupper (lower[i]);
  r[len] = '\0';
  return r;
}
