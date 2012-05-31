dnl Copyright (C) 2003,2009 Ben K. Bullock
dnl This file is part of Cfunctions.
dnl It generates the configure script for Cfunctions with GNU autoconf.

AC_INIT
AC_CONFIG_SRCDIR([src/cfunctions.fl])
AC_CONFIG_HEADER(src/defines.h)

AC_PROG_MAKE_SET

# Get the version variables from the following file:

. $srcdir/doc/VERSION

AC_CHECK_PROG(FLEX, flex, flex)
CWD=`pwd`
AC_CHECK_PROG(CFUNCTIONS, cfunctions, cfunctions,
              "$CWD/fake-cfunctions")
AC_CHECK_PROG(BZIP2, bzip2, bzip2)
AC_PATH_PROG(PERL, perl, perl)
AC_PROG_INSTALL

AC_PROG_CC

if test $CC = "gcc"; then
    echo -n "Setting CFLAGS to "
    CFLAGS='-g3 -Wall'
    echo $CFLAGS

# Special CFLAGS for compiling the Flex output.

# Flex has a goto label called `find_rule' which Cfunctions does not
# need for some reason.  The -Wno-unused is just to stop warnings
# about this, so that people do not send bug reports about it.

# Flex has a lot of flags for turning unused functions off but
# unfortunately none for turning off warnings about this.

# If you want to debug Cfunctions you should get rid of `-Wno-unused'
# and then just ignore the warning about `find_rule'.

    FLEX_CFLAGS="-Wno-unused"
else
    FLEX_CFLAGS=""
fi

# These are checks for files that the C preprocessor part of Cfunctions
# needs to operate.

AC_PROG_CPP
AC_HEADER_SYS_WAIT
AC_CHECK_HEADERS(unistd.h)

AC_REPLACE_FUNCS(getopt_long)

AC_CHECK_PROG(GCOV, gcov, gcov)

AC_CHECK_PROG(MAKEINFO, makeinfo, makeinfo)
if test $MAKEINFO; then
   DOC_TARGETS="$DOC_TARGETS info"
fi

AC_CHECK_PROG(TEXI2HTML, texi2html, texi2html)
if test $TEXI2HTML; then
    DOC_TARGETS="$DOC_TARGETS html"
fi

AC_ARG_WITH(coverage,,coverage="yes")

if test $GCOV && test $coverage; then
   echo "Coverage testing turned on"
   CFLAGS="$CFLAGS -fprofile-arcs -ftest-coverage"
   DOC_TARGETS=""
else
   CPPFLAGS="$CPPFLAGS -DCFUNCTIONS_DEBUG"
fi
AC_SUBST(CPPFLAGS)
AC_SUBST(FLEX_CFLAGS)


AC_SUBST(DOC_TARGETS)

AC_REPLACE_FUNCS(strstr)
AC_REPLACE_FUNCS(strerror)
AC_REPLACE_FUNCS(strdup)

# Texinfo needs @@ to mean @.

TEXIMAINTAINER=`echo $MAINTAINER | sed -e s/@/@@/`
AC_SUBST(TEXIMAINTAINER)

# Perl needs \@ to mean @.
# Somehow sed cannot put a backslash in the substitution pattern.

PERL_MAINTAINER=`echo $MAINTAINER | sed -e s/@/^/`
AC_SUBST(PERL_MAINTAINER)

# The following are for `cfunctions.lsm'

DATE=`date | tr a-z A-Z`
MAN_DATE=`date +"%d %b %Y"`

dnl changequote(<<, >>)dnl
dnl LSM_DATE=`echo $DATE | sed -e "s/[A-Z]\{3\} \([A-Z]\{3\}\) \([0-9]\+\) .*19\(9[89]\)/\2\1\3/g"`
dnl changequote([, ])dnl
dnl AC_SUBST(LSM_DATE)
AC_SUBST(DATE)
AC_SUBST(MAN_DATE)

URL=$CFUNCTIONS_URL
AC_SUBST(URL)

AC_SUBST(CWD)

# Do all substitutions

AC_SUBST(VERSION)
AC_SUBST(CFUNCTIONS_MINOR_VERSION)
AC_SUBST(CFUNCTIONS_MAJOR_VERSION)
AC_SUBST(MAINTAINER)
AC_SUBST(CFUNCTIONS_URL)
AC_SUBST(GCC)
AC_SUBST(COPYRIGHT_YEAR)

AC_CONFIG_FILES([Makefile doc/version.texi tests/test.perl doc/index.html 
          src/config.h doc/man.head doc/man.tail src/Makefile doc/Makefile])
AC_OUTPUT

chmod 0555 tests/test.perl
