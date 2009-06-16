#ifndef CONFIG_H
#define CONFIG_H

#define VERSION "0.26"
#define COPYRIGHT_YEAR "2009"
#define CFUNCTIONS_URL "http://cfunctions.sourceforge.net/"
#define GETOPT_LONG @GETOPT_LONG@
#define C_EX_FILE_NAME "c-extensions.h"
#define C_EXTENSIONS_FILE "/usr/local/share/cfunctions/"C_EX_FILE_NAME
#define C_EX_LOCAL "/share/projects/cfunctions/current/src/"C_EX_FILE_NAME

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
