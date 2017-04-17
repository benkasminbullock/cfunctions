#ifndef CFH_FILE_H
#define CFH_FILE_H
int fcopy (FILE * out, const char * in_file_name);
int fdiff (const char * a_name, const char * b_name);
int fexists (const char * file_name);
void freopen_stdin (const char * file_name);
void freopen_stdout (const char * file_name);
#endif 
