# contains code contributed by Egil Kvaleberg.

prefix = /home/ben/software/install
exec_prefix = ${prefix}
bindir = ${exec_prefix}/bin
sharedir = ${prefix}/share/cfunctions

SHELL=/bin/sh
FLEX = flex
CC = gcc
CPPFLAGS = -I. 
CFLAGS = -g -Wall -O
VERSION = 1.0
GCOV = 
INSTALL = /usr/bin/install -c
CWD = $(shell pwd)
srcdir = .
LIB_DIR = $(srcdir)/missing
FLEX_CFLAGS = -Wno-unused
MKDEP = gcc $(CPPFLAGS) -MM
RC = rc
CFUNCTIONS = @CFUNCTIONS@
SOURCES =  \
argument.c \
backup.c \
c-word.c \
cfunctions.c \
cpp.c \
error-msg.c \
file-name.c \
file.c \
options.c \
sys_or_exit.c \
wt.c

OBJS =  \
argument.o \
backup.o \
c-word.o \
cfunctions.o \
error-msg.o \
file-name.o \
file.o \
options.o \
sys_or_exit.o \
wt.o

HEADERS =  \
argument.h \
backup.h \
c-word.h \
cfunctions.h \
error-msg.h \
file-name.h \
file.h \
options.h \
sys_or_exit.h \
wt.h

all: cfunctions 

cfunctions:	$(OBJS)
	$(CC) $(CFLAGS) -o cfunctions $(OBJS)

cfunctions.c:	cfunctions.fl
	if [ -f $@ ]; then chmod 0644 $@; fi
	$(FLEX) -o cfunctions.c cfunctions.fl
	chmod 0444 $@

clean:	
	rm -f cfunctions $(OBJS) options TAGS

install:	all
	$(INSTALL) cfunctions $(bindir)
	if test ! -d ${sharedir} ; then \
          mkdir ${sharedir} ;\
	fi
	$(INSTALL) -m 644 c-extensions.h ${sharedir}

uninstall:
	rm -rf ${sharedir}
	rm -f ${bindir}/cfunctions

argument.o: argument.c argument.h wt.h sys_or_exit.h error-msg.h

backup.o: backup.c backup.h sys_or_exit.h error-msg.h

c-word.o: c-word.c c-word.h 

cfunctions.o: cfunctions.c argument.h wt.h error-msg.h

error-msg.o: error-msg.c error-msg.h

file.o: file.c file.h error-msg.h 

options.o: options.c options.h sys_or_exit.h  options.h config.h file.h

wt.o: wt.c config.h error-msg.h  sys_or_exit.h file.h file-name.h \
	options.h backup.h argument.h

test:
	cd tests;prove --nocolor test.pl
