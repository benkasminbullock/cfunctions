#!/home/ben/software/install/bin/perl
use warnings;
use strict;
use FindBin '$Bin';
use Deploy 'do_system';
# Hasten the day we are rid of autotools.
do_system ("cd $Bin;autoconf-2.62");
do_system ("$Bin/configure --prefix=/home/ben/software/install > /dev/null");
do_system ("make -C $Bin");
do_system ("make -C $Bin test");
