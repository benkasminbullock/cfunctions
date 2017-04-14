
#ifndef CFH_BACKUP_H
#define CFH_BACKUP_H
enum backup_type {
	none,
	simple,
	numbered_existing,
	numbered
    };
extern enum backup_type backup_type;
extern const char * simple_backup_suffix;
enum backup_type get_version (char * version);
char * basename (const char *name);
char * dirname (const char * path);
char * find_backup_file_name (char *file);
#endif 
