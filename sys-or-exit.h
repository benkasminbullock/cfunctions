#ifndef CFH_SYS_OR_EXIT_H
#define CFH_SYS_OR_EXIT_H
void * malloc_or_exit (size_t s);
void * calloc_or_exit (size_t s, size_t size);
void * realloc_or_exit (void * v, size_t s);
void fseek_or_exit (FILE * file, long offset, int whence);
void fread_or_exit (void * buffer, size_t size, size_t nmemb, FILE * stream);
void fwrite_or_exit (const void * buffer, size_t size, size_t nmemb, FILE * stream);
FILE * fopen_or_exit (const char * path, const char * mode);
void stat_or_exit (const char * file_name, struct stat * buf);
void fclose_or_exit (FILE * f);
#endif /* CFH_SYS_OR_EXIT_H */
