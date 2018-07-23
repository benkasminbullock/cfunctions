package NewTests;
use parent Exporter;
our @EXPORT_OK = qw/$binary cfwriterun cfrun/;
our %EXPORT_TAGS = (all => \@EXPORT_OK);
use warnings;
use strict;
use utf8;
use FindBin '$Bin';
use Carp;
use File::Slurper qw/read_text write_text/;
use Deploy 'do_system';

my $dir = __FILE__;
$dir =~ s!NewTests.pm$!! or die "Bad file name";
our $binary = "$dir/../cfunctions";
die "No binary $binary" unless -f $binary && -x $binary;

sub cfrun
{
my ($cfile) = @_;
if ($cfile !~ /(.*)\.c$/) {
warn "$cfile has wrong pattern, use 'file.c'";
return undef;
}
my $pattern = $1;
my $hfile = "$pattern.h";
do_system ("$binary $cfile");
if (! -f $hfile) {
warn "$hfile not created";
return undef;
}
my $hcontents = read_text ($hfile);
unlink $hfile or die $!;
return $hcontents;
}

=head2 cfwriterun

    my $h = cfwriterun ($contents, $cfile);

Write $contents to $cfile, run cfunctions on the given file, return
the output h file contents. Removes the h file as well.

=cut

sub cfwriterun
{
my ($contents, $cfile) = @_;
if (-f $cfile) {
warn ("$cfile already exists, not overwriting");
return undef;
}
write_text ($cfile, $contents);
die "No $cfile" unless -f $cfile;
my $houtput = cfrun ($cfile);
unlink ($cfile) or die $!;
return $houtput;
}


1;

