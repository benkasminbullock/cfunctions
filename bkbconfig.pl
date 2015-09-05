#!/home/ben/software/install/bin/perl
use warnings;
use strict;
use FindBin '$Bin';
use Deploy 'do_system';
do_system ("$Bin/configure --prefix=/home/ben/software/install > /dev/null");
do_system ("make -C $Bin");
do_system ("make -C $Bin test");
