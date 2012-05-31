/* Do tag table stuff for Cfunctions. */

#include <stdlib.h>
#include <string.h>
#include "error-msg.h"
#include "sys_or_exit.h"
#include "tags.h"
#include "c-word.h"
#include "wt.h"

#ifdef HEADER

enum tag_type
{
  TAG_FUNCTION,

  /* Any of `struct', `union' or `enum' names will be preceeded by the
     appropriate keyword so they can all be put into one category. */

  TAG_STRUCT_UNION_ENUM,
  TAG_MACRO,
  TAG_ENUM_VAL,
  TAG_TYPEDEF,
  TAG_GLOBAL,
  N_TAG_TYPES
};

#endif /* HEADER */

const char * tag_type_name [ N_TAG_TYPES ] =
{
  "function",
  "struct, union or enum",
  "macro",
  "enum_val",
  "typedef",
  "global"
};

extern int yylineno;
extern unsigned seen_extern;

/* File to write tag table to. */

static FILE * tag_file;
static const char * tag_file_name = "CFTAGS";
static int tags; /* -t option */

#ifdef CFUNCTIONS_DEBUG
int tag_debug;
#endif /* CFUNCTIONS_DEBUG */

void
tag_init (void)
{
  tag_file = fopen_or_exit ( tag_file_name, "w" );
  tags = 1;
}

void
tag_exit (void)
{
  if (tag_file)
    fclose (tag_file);
}
