
#ifndef CFH_BACKUP_H
#define CFH_BACKUP_H
/* When to make backup files. */

enum backup_type {
    /* Never make backups. */
    none,

    /* Make simple backups of every file. */
    simple,

    /* Make numbered backups of files that already have numbered backups,
       and simple backups of the others. */
    numbered_existing,

    /* Make numbered backups of every file. */
    numbered
};

extern enum backup_type backup_type;
extern const char * simple_backup_suffix;
enum backup_type get_version (char * version);
char * basename (const char *name);
char * dirname (const char * path);
char * find_backup_file_name (char *file);
#endif 
