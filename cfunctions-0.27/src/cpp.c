/* Start and stop C preprocessor for Cfunctions. */

/* 
   Copyright (C) 1998  Ben K. Bullock

   Cfunctions is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   Cfunctions is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with Cfunctions; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#include "config.h"

/* GNU autoconf is demented so hack around the trouble by conditional 
   compilation. */

#ifdef DISABLE_CPP
#ifdef HAS_WARNING
#warning C preprocessing with the -C option is switched
#warning off. This is because it cannot be compiled.  
#warning Your system lacks either <unistd.h> or
#warning <sys/wait.h> header files.
#endif

#else

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include "error_msg.h"
#include "sys_or_exit.h"
#include "cpp.h"

/* The process id of the pre-processor. */

static int cpp_pid;

/* Arguments to the C preprocessor. */

static char ** cpp_arguments;
static unsigned n_cpp_arguments, cpp_arguments_size = 4;

/* Increment the size of the arguments to the C preprocessor. */

static void
cpp_inc_argument (void)
{
  n_cpp_arguments++;
  if (cpp_arguments_size <= n_cpp_arguments)
    {
      cpp_arguments_size *= 2;
      cpp_arguments = realloc_or_exit (cpp_arguments, sizeof (char **) * 
                                       cpp_arguments_size);
    }
}

/* Add arguments to the C preprocessor from a list. */

void
cpp_add_argument (const char * argument)
{
  char * c;
  c = strdup (argument);
  c = strtok (c, " \t\n\r");
  if (! cpp_arguments)
    cpp_arguments = 
      malloc_or_exit (cpp_arguments_size * sizeof (char **));
  while (c)
    {
      cpp_arguments [n_cpp_arguments] = c;
      cpp_inc_argument ();
      c = strtok (NULL, " \t\n\r");
    }
}

/* Start the C preprocessor on the named file and return a file
   pointer which is the pipe output.  */

FILE * 
cpp_start (char * file_name)
{
  int pipe_descriptors[2];
  if (file_name)
    cpp_add_argument (file_name);
  else
    cpp_add_argument ("-");

  cpp_inc_argument ();
  cpp_arguments [n_cpp_arguments-1] = NULL;

  if (pipe (pipe_descriptors) == -1)
    error ("cannot pipe: %s", strerror (errno));
  cpp_pid = fork();
  if (cpp_pid == -1)
    error ("cannot fork: %s", strerror (errno));
  if (cpp_pid)
    close (pipe_descriptors[1]);
  else  /* This process is the C preprocessor. */
    {
      close (1);
      if (dup (pipe_descriptors[1]) == -1)
        error ("cannot dup: %s", strerror (errno));
      close (pipe_descriptors[1]);
      close (pipe_descriptors[0]);
      execvp (cpp_arguments[0], cpp_arguments);
      error ("cannot execvp `%s': %s", cpp_arguments[0], strerror (errno));
    }
  return (fdopen (pipe_descriptors[0], "r"));
}

/* Close the file to the preprocessor.  The return value is the status
   returned by `waitpid'. */

int
cpp_stop (FILE * f)
{
  int ret;
  
  waitpid (cpp_pid, & ret, 0);
  fclose (f);
  return ret;
}

#endif /* ! defined (DISABLE_CPP) */
