use warnings;
use strict;
use utf8;
use FindBin '$Bin';
use Test::More;
use Deploy 'do_system';
use File::Slurper 'read_text';
my $binary = "$Bin/../cfunctions";
die unless -f $binary && -x $binary;
my $h = "$Bin/struct-enum.h";
if (-f $h) {
    unlink $h or die $!;
}
do_system ("$binary $Bin/struct-enum.c");
ok (-f $h, "made a header file");
my $text = read_text ($h);
if (-f $h) {
    unlink $h or die $!;
}
like ($text, qr/enum\s*\{[^}]*\}/, "enum preserved");

done_testing ();
