#!/home/ben/software/install/bin/perl

# Strip out the line directives from the cfunctions-generated header
# files. This is because we don't want to rebuild these each time from
# the C files, but leave them to be edited by hand, so that a circular
# dependence is not created.

use warnings;
use strict;
use utf8;
use FindBin '$Bin';
use File::Slurper qw/read_text write_text/;
use C::Tokenize '$comment_re';
my @hfiles = <$Bin/*.h>;
for my $hfile (@hfiles) {
    # This is not a cfunctions generated file.
    if ($hfile =~ /^(defines|config)\.h$/) {
	next;
    }
    my $text = read_text ($hfile);
    # Remove line directives
    $text =~ s/\n#line.*\n//g;
    # Remove comments
    $text =~ s/$comment_re//g;
    # Remove multiple lines.
    $text =~ s/\n(\s*\n)+/\n/g;
    # Remove cfunctions artefacts.
    $text =~ s/\s+([,\)])/$1/g;
    write_text ($hfile, $text);
}
