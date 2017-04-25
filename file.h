#ifndef CFH_FILE_H
#define CFH_FILE_H
void fcopy (FILE * out, const char * in_file_name);

/* Various return statuses from fdiff. */

#define A_B_SAME_FILE -5
#define NO_A_FILE -2
#define NO_B_FILE -1
#define A_B_DIFFERENT 1
#define A_B_SAME 0

int fdiff (const char * a_name, const char * b_name);
int fexists (const char * file_name);
void file_memory_check ();
#endif 
