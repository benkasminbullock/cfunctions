/* Checks for C words and reserved words. */

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

#include "c-word.h"
#include <string.h>

/* 
   Determine the length of a word that could be a C word. 

   Return value: number of chars from start of x that could be a C
   word.
*/

unsigned c_word ( unsigned char * x )
{
  static unsigned char c_word_table [256] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 
    0, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 
    2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 0, 0, 0, 0, 2, 
    0, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 
    2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
  };
  unsigned i = 0;

  while ( x[i] != '\0' )
    switch ( c_word_table [ x[i] ] )
      {
      case 0: /* cannot be part of a c word */
        return i;
      case 1: /* digits */
        if ( ! i )
          return i;
      case 2: /* letters and `_' */
        i++;
      }
  return i;
}

/* This table of reserved words is taken from the file `c-parse.gperf'
   of GNU C 2.8.0. */

const char * gnu_c_keywords[] = 
{
  "__alignof",
  "__alignof__",
  "__asm",
  "__asm__",
  "__attribute",
  "__attribute__",
  "__complex",
  "__complex__",
  "__const",
  "__const__",
  "__extension__",
  "__imag",
  "__imag__",
  "__inline",
  "__inline__",
  "__iterator",
  "__iterator__",
  "__label__",
  "__real",
  "__real__",
  "__signed",
  "__signed__",
  "__typeof",
  "__typeof__",
  "__volatile",
  "__volatile__",
  "asm",
  "auto",
  "break",
  "bycopy", 
  "case",
  "char",
  "const",
  "continue",
  "default",
  "do",
  "double",
  "else",
  "enum",
  "extern",
  "float",
  "for",
  "goto",
  "id",
  "if",
  "in",
  "inout",
  "inline",
  "int",
  "long",
  "oneway",
  "out",
  "register",
  "return",
  "short",
  "signed",
  "sizeof",
  "static",
  "struct",
  "switch",
  "typedef",
  "typeof",
  "union",
  "unsigned",
  "void",
  "volatile",
  "while"
};

const unsigned n_gnu_c_keywords =sizeof ( gnu_c_keywords ) / sizeof ( char * );

/* 
   Given a string, see if it is a GNU C keyword.

   Return value: the number of the keyword in the `gnu_c_keywords'
   array if it is a keyword or -1 if not.

   Deficiency: uses binary search.  Could be much faster.  This is
   only for debugging though.  
*/

int
is_keyword ( unsigned char * may_be_keyword )
{
  int c;
  unsigned i;
  unsigned i_min = 0, i_max = n_gnu_c_keywords;

  while ( 1 )
    {
      i = i_min + (i_max - i_min)/2;
      if ( i == i_min || i == i_max )
        return -1;
      c = strcmp ( (char *) gnu_c_keywords [ i ], (char *) may_be_keyword );

      if ( c < 0 )
        i_min = i;
      else if ( c > 0 )
        i_max = i;
      else /* matched */
        return i;
    }
}

#if 0

/* The following code generated `c_word_table'. */

#include <ctype.h>
#include <stdio.h>

main() 
{ 
  int i; 

  printf ( "unsigned char c_word_table [256] = {\n" );

  for ( i = 0; i < 256; i++ ) 
  {
    if ( isalpha ( i ) || i == '_' )
      printf( "2");
    else if ( isdigit ( i ) )
      printf ( "1" );
    else
      printf ( "0" );
    if ( i != 255 ) 
      printf (", ");
    if ( i % 16 == 15 ) 
      printf ("\n");
  }
  printf ("};\n");
}

#endif /* 0 */
