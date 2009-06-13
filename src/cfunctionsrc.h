/* Structure to read in from the `.cfunctionsrc' file. */

#ifndef CFUNCTIONSRC_H
#define CFUNCTIONSRC_H

struct cfunctionsrc
{
  char * advert;
  int backup;
  int copy_c_ex;
  char * global_macro;
  int include_c_ex;
  int keep;
  int line_numbers;
  char * local_macro;
  char * proto_macro;
  char * backup_suffix;
  char * version_control;
  int write_comments;
  char * warn;
};

#endif /* CFUNCTIONSRC_H */
