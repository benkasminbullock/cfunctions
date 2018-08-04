use warnings;
use strict;
use utf8;
use FindBin '$Bin';
use Test::More;
my $builder = Test::More->builder;
binmode $builder->output,         ":utf8";
binmode $builder->failure_output, ":utf8";
binmode $builder->todo_output,    ":utf8";
binmode STDOUT, ":encoding(utf8)";
binmode STDERR, ":encoding(utf8)";
BEGIN: {
use lib "$Bin";
use NewTests ':all';
}
my $cfile = 'line-numbers.c';
my $content = <<EOF;
#ifdef HEADER
extern int mumbo;
#endif
#include "line-numbers.h"
#include <stdio.h>
int jumbo = 9999;
void tricky ()
{
printf ("%d\n", jumbo);
}
int main()
{
tricky ();
return 0;
}
EOF
my $h = cfwriterun ($content, $cfile);
like ($h, qr/#line/);
print "$h\n";
TODO: {
    local $TODO = 'Implement outgoing line numbers (bugzilla 1916)';
    unlike ($h, qr/#line 0/);
    like ($h, qr/#line \d+ "line-numbers.h"/);
};
done_testing ();
