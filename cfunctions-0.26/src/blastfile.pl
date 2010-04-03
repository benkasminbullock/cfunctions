#/usr/local/bin/perl
use warnings;
use strict;
my $top_part=<<'EOF';
# contains code contributed by Egil Kvaleberg.

prefix = @prefix@
exec_prefix = @exec_prefix@
bindir = @bindir@
sharedir = ${prefix}/share/cfunctions

SHELL=/bin/sh
FLEX = @FLEX@
CC = @CC@
CFLAGS = @CFLAGS@
CPPFLAGS = -I. @CPPFLAGS@
VERSION = @VERSION@
GCOV = @GCOV@
INSTALL = @INSTALL@
LIBOBJS = @LIBOBJS@
LIBOBJS := $(patsubst %.o,../missing/%.o,$(LIBOBJS))
CWD = $(shell pwd)
srcdir = @srcdir@
LIB_DIR = $(srcdir)/missing
FLEX_CFLAGS = @FLEX_CFLAGS@
MKDEP = gcc $(CPPFLAGS) -MM
RC = rc
CFUNCTIONS = @CFUNCTIONS@
EOF

my $bottom_part=<<'EOF';
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
EOF

my @c_files = <*.c>;
my @o_files = @c_files;
for (@o_files) { s/\.c$/.o/ }
my @h_files = @c_files;
for (@h_files) { s/\.c$/.h/ }


open my $output, ">", "Makefile.in.test" or die $!;
print $output $top_part;
print_files (\@c_files, $output, "SOURCES");
print_files (\@o_files, $output, "OBJS");
print_files (\@h_files, $output, "HEADERS");
print $output $bottom_part;
exit (0);

sub print_files
{
    my ($files, $output, $name) = @_;
    print $output join (" \\\n", ("$name = ", @$files)), "\n\n";
}
