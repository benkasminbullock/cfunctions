#ifndef CFH_OPTIONS_H
#define CFH_OPTIONS_H
#include <getopt.h>
extern struct option long_options[];
extern int n_options;
extern const char * usage[];
char * short_options (struct option * long_options, unsigned n_options);
#endif 
