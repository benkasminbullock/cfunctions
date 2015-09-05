#!/home/ben/software/install/bin/perl
use warnings;
use strict;
use Deploy 'do_system';
do_system ("./configure --prefix=/home/ben/software/install > /dev/null");
do_system ("make");
do_system ("make test");
