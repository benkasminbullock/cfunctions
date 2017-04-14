#!/home/ben/software/install/bin/perl
use warnings;
use strict;
use utf8;
use FindBin '$Bin';
use File::Slurper qw/read_text write_text/;
use File::Versions 'make_backup';
my @files = <$Bin/*.[ch]>;
for my $file (@files) {
my $text = read_text ($file);
if ($text =~ /#ifdef\s*CFUNCTIONS_DEBUG/) {
print "$file matches.\n";
$text =~ s/^#ifdef\s*CFUNCTIONS_DEBUG([^#]+)^#endif/$1/gsm;
#print $text;
make_backup ($file);
write_text ($file, $text);
}
}
exit;
