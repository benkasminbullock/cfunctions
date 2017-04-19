
#ifndef CFH_BACKUP_H
#define CFH_BACKUP_H
extern const char * simple_backup_suffix;
char * find_backup_file_name (char *file);
void free_backup_name (char * backup_name);
void backup_memory_check ();
#endif 
