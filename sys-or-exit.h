#ifndef CFH_SYS_OR_EXIT_H
#define CFH_SYS_OR_EXIT_H
#include <sys/stat.h>

typedef enum
{
    or_exit_ok,
    or_exit_null_pointer,
}
or_exit_t;

void * malloc_or_exit (size_t s);
void * calloc_or_exit (size_t s, size_t size);
void * realloc_or_exit (void * v, size_t s);
void fseek_or_exit (FILE * file, long offset, int whence);
void fread_or_exit (void * buffer, size_t size, size_t nmemb, FILE * stream);
void fwrite_or_exit (const void * buffer, size_t size, size_t nmemb, FILE * stream);
FILE * fopen_or_exit (const char * path, const char * mode);
void stat_or_exit (const char * file_name, struct stat * buf);
void fclose_or_exit (FILE * f);
or_exit_t free_or_exit (void * memory);
void memory_check ();
char * strdup_or_exit (const char * todup);
void chmod_or_exit (const char * path, mode_t mode);
void unlink_or_exit (const char * path);

#define CALLX(x) {						\
	or_exit_t status = x;					\
	if (status != or_exit_ok) {				\
	    fprintf (stderr,					\
		     "%s:%d: %s failed with status %d\n",	\
		     __FILE__, __LINE__, #x, status);		\
	    exit (EXIT_FAILURE);				\
	}							\
    }

#endif /* CFH_SYS_OR_EXIT_H */
