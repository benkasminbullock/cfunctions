prefix = /home/ben/software/install
exec_prefix = ${prefix}
bindir = ${exec_prefix}/bin
sharedir = ${prefix}/share/cfunctions

FLEX = flex
CFLAGS = -g -Wall
#CFLAGS = -g -Wall -O
INSTALL = /usr/bin/install -c
SOURCES =  \
argument.c \
backup.c \
cfunctions.c \
cpp.c \
error-msg.c \
file.c \
sys-or-exit.c \
wt.c

OBJS =  \
argument.o \
backup.o \
cfunctions.o \
error-msg.o \
file.o \
sys-or-exit.o \
wt.o

HEADERS =  \
argument.h \
backup.h \
cfunctions.h \
error-msg.h \
file.h \
sys-or-exit.h \
wt.h

all: cfunctions 

cfunctions:	$(OBJS)
	$(CC) $(CFLAGS) -o cfunctions $(OBJS)

argument.o: argument.c argument.h wt.h sys-or-exit.h error-msg.h

backup.o: backup.c backup.h sys-or-exit.h error-msg.h

cfunctions.o: cfunctions.c argument.h wt.h error-msg.h flex.c flex.h

error-msg.o: error-msg.c error-msg.h

file.o: file.c file.h error-msg.h 

wt.o: wt.c config.h error-msg.h  sys-or-exit.h file.h \
	backup.h argument.h cfunctions.h flex.h

cfunctions.h: cfunctions.fl
	$(FLEX) --header-file=cfunctions.h -o cfunctions.c cfunctions.fl

sys-or-exit.o: sys-or-exit.c sys-or-exit.h error-msg.h

cfunctions.c:	cfunctions.fl
	if [ -f $@ ]; then chmod 0644 $@; fi
	$(FLEX) --header-file=cfunctions.h -o cfunctions.c cfunctions.fl
	chmod 0444 $@

clean:	
	rm -f cfunctions $(OBJS) cfunctions.[ch] \
	test-file test-file.o c-tap-test.h

install:	all
	$(INSTALL) cfunctions $(bindir)
	if test ! -d ${sharedir}; then mkdir ${sharedir}; fi
	$(INSTALL) -m 644 c-extensions.h ${sharedir}

uninstall:
	rm -rf ${sharedir}
	rm -f ${bindir}/cfunctions

test:   cfunctions unittest
	cd tests;prove --nocolor test.pl
	prove --nocolor newtests/*.t

unittest: test-file
	prove --nocolor ./test-file

test-file: test-file.o sys-or-exit.o error-msg.o
	$(CC) $(CFLAGS) -o test-file test-file.o sys-or-exit.o error-msg.o

test-file.o: file.c file.h c-tap-test.h sys-or-exit.h error-msg.h
	$(CC) -c $(CFLAGS) -o test-file.o -DTEST file.c

# The following file can be obtained from
# https://www.lemoda.net/c/simple-tap-test/index.html

CTT=/home/ben/projects/c-tap-test/c-tap-test.h

c-tap-test.h: $(CTT)
	if [ -f $@ ]; then chmod 0644 $@; fi
	cp -f $(CTT) $@
	chmod 0444 $@
