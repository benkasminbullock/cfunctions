# contains code contributed by Egil Kvaleberg.

prefix = /usr/local
exec_prefix = ${prefix}
bindir = ${exec_prefix}/bin
sharedir = ${prefix}/share/cfunctions

SHELL=/bin/sh
FLEX = flex
CC = gcc
CPPFLAGS = -I.  -DCFUNCTIONS_DEBUG
CFLAGS = -g3 -Wall  -DCFUNCTIONS_DEBUG
VERSION = 0.27
GCOV = gcov
INSTALL = /usr/bin/install -c
LIBOBJS = 
LIBOBJS := $(patsubst %.o,../missing/%.o,$(LIBOBJS))
CWD = $(shell pwd)
srcdir = .
LIB_DIR = $(srcdir)/missing
FLEX_CFLAGS = -Wno-unused
MKDEP = gcc $(CPPFLAGS) -MM
RC = rc
CFUNCTIONS = cfunctions
SOURCES =  \
argument.c \
backup.c \
c-word.c \
cfunctions.c \
cpp.c \
error_msg.c \
file-name.c \
file.c \
name-convert.c \
options.c \
print_usage.c \
rc-rw.c \
sys_or_exit.c \
tags.c \
traditional.c \
wt.c

OBJS =  \
argument.o \
backup.o \
c-word.o \
cfunctions.o \
cpp.o \
error_msg.o \
file-name.o \
file.o \
name-convert.o \
options.o \
print_usage.o \
rc-rw.o \
sys_or_exit.o \
tags.o \
traditional.o \
wt.o

HEADERS =  \
argument.h \
backup.h \
c-word.h \
cfunctions.h \
cpp.h \
error_msg.h \
file-name.h \
file.h \
name-convert.h \
options.h \
print_usage.h \
rc-rw.h \
sys_or_exit.h \
tags.h \
traditional.h \
wt.h

# if you get into trouble, just delete the following rule.

.c.h:
	$(CFUNCTIONS) -inmb $<

all: cfunctions 

cfunctions:	$(OBJS) $(LIBOBJS)
	$(CC) $(CFLAGS) -o $(srcdir)/cfunctions $(OBJS) $(LIBOBJS) 

cfunctions.c:	cfunctions.fl
	$(FLEX) -o$(srcdir)/cfunctions.c $(srcdir)/cfunctions.fl

rc-rw.c:	cfunctionsrc.h
	$(RC) cfunctionsrc.h

OPTIONS_FILES = sys_or_exit.o error_msg.o file.o
OPTIONS_INCLUDES = sys_or_exit.h error_msg.h c-extensions.h \
		 options.h config.h file.h

options:        options.c $(OPTIONS_FILES) $(OPTIONS_INCLUDES)
	$(CC) $(CPPFLAGS) $(CFLAGS) -o options -DMANUAL \
	options.c $(OPTIONS_FILES) $(LIBOBJS)

.c.gcov:
	$(GCOV)	-b $<

coverage:	*.gcov

# This target removes extra coverage files

check-clean:
	rm -f 	*.da *.bb *.bbg *.gcov coverage

clean:	
	rm -f cfunctions $(OBJS) $(LIBOBJS) options TAGS

distclean:	clean
	rm -f defines.h man.head Makefile config.h

realclean:	distclean
	rm -f cfunctions.c configure version.texi options.texi LSM \
	index.html $(HEADERS)

# Rules for functions which might be missing from the operating system.

strstr.o:	$(LIB_DIR)/strstr.c

getopt_long.o:	$(LIB_DIR)/getopt_long.c

install:	all
	$(INSTALL) cfunctions $(bindir)
	if test ! -d ${sharedir} ; then \
          mkdir ${sharedir} ;\
	fi
	$(INSTALL) -m 644 c-extensions.h ${sharedir}

uninstall:
	rm -rf ${sharedir}
	rm -f ${bindir}/cfunctions

dependencies:	$(SOURCES) $(HEADERS)
	$(MKDEP) $(SOURCES) > $@

include dependencies
