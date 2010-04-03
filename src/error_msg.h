/* This is a Cfunctions (version 0.26) generated header file.
   Cfunctions is a free program for extracting headers from C files.
   Get Cfunctions from `http://cfunctions.sourceforge.net/'. */

/* This file was generated with:
`cfunctions -inmb error_msg.c' */
#ifndef CFH_ERROR_MSG_H
#define CFH_ERROR_MSG_H

/* From `error_msg.c': */

#line 26 "error_msg.c"
#include <stdio.h>
#include <errno.h>
#include <string.h>
#define HERE __FILE__, __LINE__
/* The name of the program running and of the source file the program
   is reading from.  The default values are just for debugging. */

#line 37 "error_msg.c"
extern const char * program_name;

#line 38 "error_msg.c"
extern const char * source_name;

#line 39 "error_msg.c"
extern int * source_line;
#include "c-extensions.h"
/* Print a warning message with a line number.  */

#line 53 "error_msg.c"
void lwarning (unsigned line_number , const char * format , ... ) X_PRINT_FORMAT(2, 3);
/* Print an error message for a specified line and then exit */

#line 65 "error_msg.c"
void line_error (const char * format , ... ) X_NO_RETURN X_PRINT_FORMAT(1, 2);
/* Print a warning message with a line number.  */

#line 79 "error_msg.c"
void line_warning (const char * format , ... ) X_PRINT_FORMAT(1, 2);
/* Print a warning message with a line number.  */

#line 91 "error_msg.c"
void line_info (const char * format , ... ) X_PRINT_FORMAT(1, 2);
/* Print an error message for a specified line and then exit */

#line 103 "error_msg.c"
void lerror (unsigned line_number , const char * format , ... ) X_NO_RETURN X_PRINT_FORMAT(2, 3);
/* Print the specified error message and exit. */

#line 117 "error_msg.c"
void error (const char * format , ... ) X_NO_RETURN X_PRINT_FORMAT(1, 2);
/* Print a warning message. */

#line 131 "error_msg.c"
void warning (const char * format , ... ) X_PRINT_FORMAT(1, 2);
/* Print an internal error message.  This is for the case that
   something that should never happen occurs, such as a `default:' in
   a switch statement that is used to catch errors. */

#line 145 "error_msg.c"
void bug (const char * file , unsigned line , const char * format , ... ) X_NO_RETURN X_PRINT_FORMAT(3, 4);

#endif /* CFH_ERROR_MSG_H */
