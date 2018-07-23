use warnings;
use strict;
use utf8;
use FindBin '$Bin';
use Test::More;
BEGIN: {
use lib "$Bin";
use NewTests ':all';
}
my $text = cfrun ("$Bin/struct-enum.c");
ok ($text);
like ($text, qr/enum\s*\{[^}]*\}/, "enum preserved");

done_testing ();
