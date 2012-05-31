/* This file reads and writes an `rc' file.  It was created by `rc'.
   Rc is available from the following URL:
   http://www.hayamasa.demon.co.uk/rc/index.html.  */

#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "error-msg.h"
#include "sys_or_exit.h"
#include "name-convert.h"
#include "cfunctionsrc.h"
#include "rc-rw.h"

/* list of stuff to search through. */

static const char * const rc_fields[] =
{
  "advert",
  "backup",
  "backup_suffix",
  "copy_c_ex",
  "global_macro",
  "include_c_ex",
  "keep",
  "line_numbers",
  "local_macro",
  "proto_macro",
  "version_control",
  "warn",
  "write_comments",

};

/* Size of the above array. */

static unsigned n_rc_fields = sizeof (rc_fields) / sizeof (char *);

/* Get the number of the field which corresponds to the data string or
   -1 if it is not there. */

static int
get_field (char * data)
{
  char * field = malloc (strlen (data) + 1);
  char * modf;
  char * end;
  unsigned i;

  strcpy (field, data);
  end = strchr (field, ':');
  if (! end)
    {
      free (field);
      return -1;
    }
  * end = '\0';
  while (!isalnum (*field))
    field++;
  modf = spaces_to_uscores (field);
  /* bsearch gives wrong answers for some weird reason. */
  for (i = 0; i < n_rc_fields; i++)
    if (strcmp (modf, rc_fields[i]) == 0)
      {
        free (modf);
        free (field);
        return i;
      }
  free (field);
  free (modf);
  return -1;
}

/* Return a pointer to where the value of the field is. */

static char *
get_value (char * data)
{
  char * value;
  value = strchr (data, ':') + 1;
  if (! value)
    return NULL;
  while (isspace (*value))
    value++;
  return value;
}

/* Initialize all the fields of the named structure to their default
   values. */

void
rc_initialize (struct cfunctionsrc * rc) 
{
  memset (rc, 0, sizeof (struct cfunctionsrc));


}

/* Read the rc file and set the values of the struct. */

void
rc_read (struct cfunctionsrc * rc, FILE * rc_file)
{
  char data[256];
  while (1)
    {
      char * line_end;
      char * value;
      int field;

      if (feof (rc_file))
        return;
      fgets (data, 256, rc_file);
      if (! strchr (data, ':'))
        continue;
      line_end = strchr (data, '\n');
      if (! line_end)
        continue;
      else
        * line_end = '\0';
      field = get_field (data);
      value = get_value (data);

      switch (field)
        {
        case -1:
          break;

        case 0: /* advert */
          rc->advert = strdup (value);
          break;

        case 1: /* backup */
          rc->backup = atoi (value);
          break;

        case 2: /* backup_suffix */
          rc->backup_suffix = strdup (value);
          break;

        case 3: /* copy_c_ex */
          rc->copy_c_ex = atoi (value);
          break;

        case 4: /* global_macro */
          rc->global_macro = strdup (value);
          break;

        case 5: /* include_c_ex */
          rc->include_c_ex = atoi (value);
          break;

        case 6: /* keep */
          rc->keep = atoi (value);
          break;

        case 7: /* line_numbers */
          rc->line_numbers = atoi (value);
          break;

        case 8: /* local_macro */
          rc->local_macro = strdup (value);
          break;

        case 9: /* proto_macro */
          rc->proto_macro = strdup (value);
          break;

        case 10: /* version_control */
          rc->version_control = strdup (value);
          break;

        case 11: /* warn */
          rc->warn = strdup (value);
          break;

        case 12: /* write_comments */
          rc->write_comments = atoi (value);
          break;



        default:
          bug (HERE, "bad value %d", field);
          break;
        }
    }
}

/* Write the rc file. */

void
rc_write (struct cfunctionsrc * rc, FILE * rc_out)
{
  /* Default values of the rc. */

  struct cfunctionsrc dv; 

  rc_initialize (& dv);

  if ((rc->advert && ! dv.advert) ||
     (dv.advert && rc->advert && strcmp (dv.advert, rc->advert) != 0))
    fprintf (rc_out, "advert: %s\n", rc->advert);
  if (dv.backup != rc->backup)
    fprintf (rc_out, "backup: %d\n", rc->backup);
  if ((rc->backup_suffix && ! dv.backup_suffix) ||
     (dv.backup_suffix && rc->backup_suffix && strcmp (dv.backup_suffix, rc->backup_suffix) != 0))
    fprintf (rc_out, "backup suffix: %s\n", rc->backup_suffix);
  if (dv.copy_c_ex != rc->copy_c_ex)
    fprintf (rc_out, "copy c ex: %d\n", rc->copy_c_ex);
  if ((rc->global_macro && ! dv.global_macro) ||
     (dv.global_macro && rc->global_macro && strcmp (dv.global_macro, rc->global_macro) != 0))
    fprintf (rc_out, "global macro: %s\n", rc->global_macro);
  if (dv.include_c_ex != rc->include_c_ex)
    fprintf (rc_out, "include c ex: %d\n", rc->include_c_ex);
  if (dv.keep != rc->keep)
    fprintf (rc_out, "keep: %d\n", rc->keep);
  if (dv.line_numbers != rc->line_numbers)
    fprintf (rc_out, "line numbers: %d\n", rc->line_numbers);
  if ((rc->local_macro && ! dv.local_macro) ||
     (dv.local_macro && rc->local_macro && strcmp (dv.local_macro, rc->local_macro) != 0))
    fprintf (rc_out, "local macro: %s\n", rc->local_macro);
  if ((rc->proto_macro && ! dv.proto_macro) ||
     (dv.proto_macro && rc->proto_macro && strcmp (dv.proto_macro, rc->proto_macro) != 0))
    fprintf (rc_out, "proto macro: %s\n", rc->proto_macro);
  if ((rc->version_control && ! dv.version_control) ||
     (dv.version_control && rc->version_control && strcmp (dv.version_control, rc->version_control) != 0))
    fprintf (rc_out, "version control: %s\n", rc->version_control);
  if ((rc->warn && ! dv.warn) ||
     (dv.warn && rc->warn && strcmp (dv.warn, rc->warn) != 0))
    fprintf (rc_out, "warn: %s\n", rc->warn);
  if (dv.write_comments != rc->write_comments)
    fprintf (rc_out, "write comments: %d\n", rc->write_comments);


}

static char * 
get_full_name (void)
{
  const char * file_name = ".cfunctionsrc";
  char * home_name, * full_name;

  home_name = getenv ("HOME");
  if (! home_name)
    {
      warning ("cannot find value of $HOME: the value "
               "should be your home directory");
      return NULL;
    }
  full_name = malloc_or_exit (strlen (home_name) + strlen (file_name) + 10);
  sprintf (full_name, "%s/%s", home_name, file_name);
  return full_name;
}

/* Read the `rc' file. */

void
read_rc_file (struct cfunctionsrc * rc)
{
  FILE * rc_file;
  char * full_name;

  full_name = get_full_name ();
  if (! full_name)
    return;
  rc_initialize (rc);
  rc_file = fopen (full_name, "r");
  free (full_name);
  if (! rc_file)
    {
      /* Only warn if there was an error, not if it just can't find
         the file, since there often won't be a file anyway. */
      if (errno != ENOENT)
        warning ("could not open %s: %s", full_name, strerror (errno));
      return;
    }
  rc_read (rc, rc_file);
  fclose (rc_file);
}

/* Does an rc file exist? */

int
rc_file_exists (void)
{
  char * full_name;
  struct stat s;
  int ret = 0;

  full_name = get_full_name ();
  if (full_name)
    {
      if (stat (full_name, & s))
        {
          if (errno != ENOENT)
            warning ("could not open `%s': %s", 
                     full_name, strerror (errno));
        }
      else
        ret = 1;
      free (full_name);
    }
  return ret;
}

/* Write the `rc' file. */

void
write_rc_file (struct cfunctionsrc * rc)
{
  FILE * rc_file;
  char * full_name;

  full_name = get_full_name ();
  if (! full_name)
    return;
  rc_file = fopen (full_name, "w");
  free (full_name);
  if (! rc_file)
    {
      /* Only warn if there was an error, not if it just can't find
         the file, since there often won't be a file anyway. */
      if (errno != ENOENT)
        warning ("could not open %s: %s", full_name, strerror (errno));
      return;
    }
  rc_write (rc, rc_file);
  fclose (rc_file);
}

