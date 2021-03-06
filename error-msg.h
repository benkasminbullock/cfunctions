#ifndef CFH_ERROR_MSG_H
#define CFH_ERROR_MSG_H
#define HERE __FILE__, __LINE__
extern int * source_line;
#include "c-extensions.h"
void lwarning (unsigned line_number, const char * format, ...) X_PRINT_FORMAT(2, 3);
void line_error (const char * format, ...) X_NO_RETURN X_PRINT_FORMAT(1, 2);
void line_warning (const char * format, ...) X_PRINT_FORMAT(1, 2);
void line_info (const char * format, ...) X_PRINT_FORMAT(1, 2);
void lerror (unsigned line_number, const char * format, ...) X_NO_RETURN X_PRINT_FORMAT(2, 3);
void error (const char * format, ...) X_NO_RETURN X_PRINT_FORMAT(1, 2);
void warning (const char * format, ...) X_PRINT_FORMAT(1, 2);
void bug (const char * file, unsigned line, const char * format, ...) X_NO_RETURN X_PRINT_FORMAT(3, 4);
void set_program_name (const char *);
#endif 
