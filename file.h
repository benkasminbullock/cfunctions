#ifndef CFH_FILE_H
#define CFH_FILE_H
void fcopy (FILE * out, const char * in_file_name);
int fdiff (const char * a_name, const char * b_name);
int fexists (const char * file_name);
void file_memory_check ();
#endif 
