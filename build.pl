#!/home/ben/software/install/bin/perl
use warnings;
use strict;
use Perl::Build;
use FindBin '$Bin';
perl_build (
    pre => "$Bin/bkbconfig.pl",
    clean => "$Bin/clean.pl",
);
