
#ifndef CFH_BACKUP_H
#define CFH_BACKUP_H
extern const char * simple_backup_suffix;
char * basename (const char *name);
char * dirname (const char * path);
char * find_backup_file_name (char *file);
#endif 
