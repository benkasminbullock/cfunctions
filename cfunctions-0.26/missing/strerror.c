/* Fake strerror. */

#include <stdio.h>
#include <config.h>

#ifdef HAS_WARNING
#warning Your system does not have strerror function.
#warning Error messages will just have numbers.
#endif

char * strerror (int errno)
{
  static char error_number [50];
  sprintf (error_number, "error number %d", errno);
  return error_number;
}
