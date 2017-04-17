#ifndef CONFIG_H
#define CONFIG_H

#define VERSION "1.0"
#define COPYRIGHT_YEAR "2015"
#define CFUNCTIONS_URL "https://github.com/benkasminbullock/cfunctions"
#define GETOPT_LONG @GETOPT_LONG@
#define C_EX_FILE_NAME "c-extensions.h"
#define C_EXTENSIONS_FILE "/home/ben/software/install/share/cfunctions/"C_EX_FILE_NAME
#define C_EX_LOCAL "/usr/home/ben/projects/cfunctions/src/"C_EX_FILE_NAME

#define CPP "gcc -E"
#define DISABLE_CPP

#ifdef __GNUC__
#define HAS_WARNING
#define ALLOCA(x) alloca(x)
#define FREE_ALLOCA(x)
#else
#define ALLOCA(x) malloc_or_exit(x)
#define FREE_ALLOCA(x) free(x)
#endif

#endif /* CONFIG_H */
