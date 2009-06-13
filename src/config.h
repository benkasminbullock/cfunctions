#ifndef CONFIG_H
#define CONFIG_H

#define VERSION "0.25"
#define COPYRIGHT_YEAR "2006"
#define CFUNCTIONS_URL "http://www.kasmin.org/cfunctions/"
#define GETOPT_LONG @GETOPT_LONG@
#define C_EX_FILE_NAME "c-extensions.h"
#define C_EXTENSIONS_FILE "/home/ben/software/install/share/cfunctions/"C_EX_FILE_NAME
#define C_EX_LOCAL "/usr/home/ben/software/cfunctions/cfunctions-0.25/src/"C_EX_FILE_NAME

#define CPP "gcc -E"

#ifdef __GNUC__
#define HAS_WARNING
#define ALLOCA(x) alloca(x)
#define FREE_ALLOCA(x)
#else
#define ALLOCA(x) malloc_or_exit(x)
#define FREE_ALLOCA(x) free(x)
#endif

#endif /* CONFIG_H */
