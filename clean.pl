#!/home/ben/software/install/bin/perl
use warnings;
use strict;
use FindBin '$Bin';
use Deploy 'do_system';
if (-f "$Bin/src/Makefile") {
    do_system ("make -C $Bin/src clean");
}
if (-f "$Bin/Makefile" && -f "$Bin/src/Makefile") {
    do_system ("make -C $Bin clean");
}

