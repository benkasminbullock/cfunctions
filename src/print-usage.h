#ifndef CFH_PRINT_USAGE_H
#define CFH_PRINT_USAGE_H
#include <getopt.h>
extern unsigned error_msg_width;
void print_usage (unsigned n_options, struct option * long_options, const char ** usage);
#endif 
