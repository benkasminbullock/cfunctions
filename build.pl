#!/home/ben/software/install/bin/perl
use warnings;
use strict;
use Perl::Build;
perl_build (
    pre => './bkbconfig.pl',
    clean => './clean.pl',
);
