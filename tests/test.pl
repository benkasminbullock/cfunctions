#!/usr/bin/env perl

# This is the master test script for cfunctions.

use warnings;
use strict;
use FindBin '$Bin';
use Test::More;
use File::Compare;

chdir $Bin or die $!;
my $cfunctions = "$Bin/../cfunctions";
if (! -x $cfunctions) {
    die "No $cfunctions; need to rebuild?";
}
my $cc         = "cc";
my $GNU_c      = 1;
main ();
done_testing ();
exit;

sub main
{

    # Make sure that the executable Cfunctions does exist.

    if ( ! -e $cfunctions || ! -x $cfunctions ) {
	print STDERR 
        "$0: '$cfunctions' cannot be found or is not executable.\n";
	exit ( 1 );
    }

    rm_h_files();

    opendir(THISDIR, ".");

    my @bad_files = grep ( /^bad-.*\.c$/, readdir THISDIR);

    closedir THISDIR;

    foreach my $bad_file (@bad_files) {
	err_test ( $bad_file, "error" );
    }

    # Try running Cfunctions on each `warn-' file and see if it generates
    # a warning message.

    opendir(THISDIR, ".");

    my @warn_files = grep ( /^warn-.*\.c$/, readdir THISDIR);

    foreach my $warn_file (@warn_files) {
	err_test ( $warn_file, "warning" );
    }

    opendir(THISDIR, ".");

    my @ok_files = grep ( /^ok-.*\.c$/, readdir THISDIR);

    foreach my $ok_file (@ok_files) {
	try_ok ( $ok_file );
    }

    rm_h_files();

    if (-f 'a.out') {
	unlink ( "a.out" ) || die;
    }
    test_backup ();
    rm_h_files ();
}

# Remove the backups

sub test_backup
{
    rm_h_files ();
    my $input = "$Bin/backup-test.c";
    my $output = $input;
    $output =~ s/\.c$/.h/;
    system ("$cfunctions $input");
    ok (-f $output, "Generated $output");
    my $backup1 = "$output.~1~";
    if (-f $backup1) {
	die "Backup file $backup1 exists";
    }
    rename $output, $backup1 or die $!;
    system ("$cfunctions $input");
    ok (-f $output, "Generated $output");
    ok (-f $backup1, "Did not clobber first backup");
    unlink $backup1 or die $!;
    unlink $output or die $!;
    write_garbage ($backup1);
    if (! -f $backup1) {
	die "Failed to create garbage $backup1 file";
    }
    system ("$cfunctions $input");
    ok (-f $output, "Generated $output");
    ok (-f $backup1, "Did not clobber first backup");
    system ("$cfunctions $input");
    my $backup2 = "$output.~2~";
    ok (! -f $backup2, "Did not make a new backup file");
    write_garbage ($output);
    system ("$cfunctions $input");
    ok ($backup2, "Made a new backup file after trashing old correct one");
    system ("$cfunctions $input");
    my $backup3 = "$output.~3~";
    ok (! -f $backup3, "Did not make an unnecessary backup file $backup3");
    for my $file ($output, $backup1, $backup2, $backup3) {
	if (-f $file) {
	    unlink $file or die $!;
	}
    }
}

sub write_garbage
{
    my ($file_name) = @_;
    if (-f $file_name) {
	unlink $file_name or die $!;
    }
    open my $out, ">", $file_name or die $!;
    print $out "Garbage for tests " . rand () . "\n";
    close $out or die $!;
}

# Remove header files, backup header files and other generated files
# from the current directory.

sub rm_h_files
{
    opendir(THISDIR, ".") or die $!;
    my @h_files = grep ( /\.h(?:\.~[0-9]+~)?$/, readdir THISDIR);

    closedir THISDIR or die $!;

    for my $h_file (@h_files) {
        unlink $h_file or die $!;
    }
}

sub err_test
{
    my $c_file = $_[0];
    my $message_type = $_[1];
    my $options = "";

    my $test = "FAILED";

# Get the error message from the C file.  Every `bad-' and `warn-'
# file should have exactly one error message in it, which is a
# substring of the error message it is supposed to trigger in
# Cfunctions.  This is marked by `$message_type'.

    open ( my $C_FILE, "<", $c_file ) or die;

    my $msg = "";

    while ( <$C_FILE> )
    {
        if ( /\/\/\s*options:\s*(.*)/ )
        {
            $options = $1;
        }
        if ( /\/\/\s*$message_type:\s*(.*)/ )
        {
            if ( $msg )
            {
                print "bad test: too many `$message_type' statements.\n";
                close $C_FILE;
                next;
            }
            $msg = $1;
        }
    }

    my $runcommand = "$cfunctions $options $c_file"; 
#    note "$runcommand: ";

    if ( ! $msg ) {
        note "bad test: no `$message_type' statements.\n"; 
    }

    close $C_FILE or die $!;

# Try running Cfunctions on the C file and see if it generates an
# error message containing the right substrings.

    open ( my $TEST_OUT, "$runcommand 2>&1 |" ) or die $!;

    while ( <$TEST_OUT> )
    {
        if ( /$message_type/ && /$msg/ )
        {
            $test = "PASSED";
        }
    }
    # Because cfunctions exited with an error status, "close" will
    # return a non-zero value.
#    close $TEST_OUT or die $!;

    unlike ( $test, qr/FAILED/, "cfunctions made message $message_type processing $c_file" );

    return;
}

# Run Cfunctions on the PASSED files.  If Cfunctions gives an error
# message, then fail the test.

sub try_ok 
{
    my $c_file = $_[0];
    my $failed = 0;
    my $options = "";
    my $error_msg = "";
    my $test = "PASSED";

# Get the options from the C file, if there are any.

    open ( C_FILE, $c_file ) || die;

    while ( <C_FILE> )
    {
        if ( /\/\/\s*options:\s*(.*)/ )
        {
            $options = $1;
        }
    }

    close C_FILE;

#    note "$cfunctions $options $c_file: "; 

# Try running Cfunctions on the C file and see if it generates an
# error message containing the right substrings.

    open ( TEST_OUT, "$cfunctions $c_file 2>&1 |" );

    while ( <TEST_OUT> ) {
	$error_msg .= $_;
    }
    close TEST_OUT;
    ok (! $error_msg, "no errors with $cfunctions $c_file");
    if ($error_msg) {
	diag ("Cfunctions produced errors as follows: $error_msg");
    }

    # Try compiling and linking using the generated header file.

    my $link_file = $c_file;
    $link_file =~ s/^ok-/link-/;
    my $cc_op = "";
    my $gcc_op;
    if ( ! $GNU_c )
    {
        $gcc_op = "";
    }
    else
    {
        $gcc_op = "-Wall ";
    }
    open ( LINK_FILE, $link_file ) || die "no link file for $c_file";

    while (<LINK_FILE>) {
        if ( /\/\/\s*options:\s*(.*)/ ) {
            $cc_op .= $1;
        }
        if ( $GNU_c ) {
            if ( /\/\/\s*gcc_opt:\s*(.*)/ ) {
                $gcc_op .= $1;
            }
        }
    }

    close LINK_FILE;

#    note "$cc $cc_op $gcc_op $c_file $link_file: "; 

    my $cc_error_msg = "";

    # The following `2>&1' directs `stderr' to Perl and `stdout' to
    # `/dev/null'.

    open ( CC_OUTPUT, '-|', 
           "$cc $cc_op $gcc_op $c_file $link_file 2>&1 >/dev/null" );

    while ( <CC_OUTPUT> ) {
        if ( /.+/ ) {
            $cc_error_msg .= $_;
        }
    }

    close CC_OUTPUT;

    ok (! $cc_error_msg, "no errors compiling $c_file and $link_file");
    if ($cc_error_msg) {
	note ("Compiler error is '$cc_error_msg'");
    }
    return;
}

