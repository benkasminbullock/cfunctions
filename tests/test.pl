#!/usr/bin/env perl

# This is the master test script for cfunctions.

use warnings;
use strict;
use FindBin '$Bin';
use Test::More;
use File::Compare;
use IPC::Run3;
use File::Slurper qw/read_lines read_text/;
use C::Utility 'c_to_h_name';

chdir $Bin or die $!;
my $cfunctions = "$Bin/../cfunctions";
# Make sure that the executable exists.
if (! -x $cfunctions) {
    die "No $cfunctions; need to rebuild?";
}
# C compiler command.
my $cc = "cc";
main ();
done_testing ();
rm_h_files ();
exit;

sub main
{
    rm_h_files();
    my @bad_files = grepdir ( qr/^bad-.*\.c$/);
    foreach my $bad_file (@bad_files) {
	err_test ($bad_file, "error");
    }
    # Try running Cfunctions on each `warn-' file and see if it generates
    # a warning message.

    my @warn_files = grepdir ( qr/^warn-.*\.c$/);
    foreach my $warn_file (@warn_files) {
	err_test ($warn_file, "warning");
    }
    my @ok_files = grepdir (qr/^ok-.*\.c$/);
    foreach my $ok_file (@ok_files) {
	try_ok ($ok_file);
    }


    rm_h_files();
    test_not_c_warning ();
    rm_h_files();
    test_backup ();
    rm_h_files ();
    test_verbatim ();
    rm_h_files ();
    test_line_numbers ();
    rm_h_files ();
    test_spacing ();
    rm_h_files ();
    test_attribute ();
    rm_h_files ();
#    test_valgrind ();
#    rm_h_files ();
}

sub test_not_c_warning
{
    my $filename = "$Bin/not-c-file-name.funkyjunk";
    rm_h_files ();
    write_garbage ($filename);
    run3 ("cfunctions $filename", undef, \my $output, \my $errors);
    like ($errors, qr/does not look like a C file/,
	  "warning for non-c-file name $filename");
    ok (! $?, "no error status for bad file name");
    if (-f $filename) {
	unlink $filename or die $!;
    }
    my $hfile = $filename;
    $hfile =~ s/.$/h/;
    ok (-f $hfile, "made the header file anyway");
    if (-f $hfile) {
	unlink $hfile or die $!;
    }
    rm_h_files ();
}

# This tests the backup facility. The backup facility has two
# functions, first of all it backs up old .h files into .h.~1~ etc,
# and secondly it un-backs-up the old .h file if the new file is the
# same as the old one (see unbackup in wt.c) so that unnecessary
# builds are not performed for unchanged files. This tests both of
# these.

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

sub grepdir
{
    my ($re) = @_;
    if (ref $re ne 'Regexp') {
	die "Not a regex";
    }
    opendir (THISDIR, $Bin) or die $!;
    my @files = grep /$re/, readdir THISDIR;
    closedir THISDIR or die $!;
    return @files;
}

# Remove header files, backup header files and other generated files
# from the current directory.

sub rm_h_files
{
    my @h_files = grepdir (qr/\.h(?:\.~[0-9]+~)?$/);
    for my $h_file (@h_files) {
        unlink $h_file or die $!;
    }
    if (-f 'a.out') {
	unlink ( "a.out" ) or die $!;
    }
}

# Test production of error and warning messages.

sub err_test
{
    my ($c_file, $message_type) = @_;

    my $text = read_text ($c_file);
    my $msg = '';
    # Get the error message from the C file.  Every `bad-' and `warn-'
    # file should have exactly one error message in it, which is a
    # substring of the error message it is supposed to trigger in
    # Cfunctions.  This is marked by `$message_type'.
    if ($text =~ m!//\s*$message_type:\s*(.*)!) {
	$msg = $1;
    }
    my $runcommand = "$cfunctions $c_file"; 
    if (! $msg) {
        die "bad test $c_file: no `$message_type' statements.\n"; 
    }

    # Try running Cfunctions on the C file and see if it generates an
    # error message containing the right substrings.

    run3 ($runcommand, undef, \my $output, \my $errors);
    if ($message_type eq 'error') {
	ok ($?, "Got error status from $c_file");
    }
    else {
	ok (! $?, "No error status from $c_file");
    }
    like ($errors, qr/\Q$message_type\E.*\Q$msg\E/);
}

# Run Cfunctions on the PASSED files.  If Cfunctions gives an error
# message, then fail the test.

sub try_ok 
{
    my $c_file = $_[0];
    my $failed = 0;
    my $error_msg = "";
    my $test = "PASSED";

    # Try running Cfunctions on the C file and see if it generates an
    # error message containing the right substrings.

    run3 ("$cfunctions $c_file", undef, \my $output, \$error_msg);
    ok (! $?, "No error status from $c_file");
    ok (! $error_msg, "no errors with $cfunctions $c_file");
    if ($error_msg) {
	diag ("Cfunctions produced errors as follows: $error_msg");
    }

    # Try compiling and linking using the generated header file.

    my $link_file = $c_file;
    $link_file =~ s/^ok-/link-/;
    my $cc_op = " -Wall";
    open my $lf, "<", $link_file or die "no link file for $c_file";

    while (<$lf>) {
        if ( /\/\/\s*options:\s*(.*)/ ) {
            $cc_op .= " $1";
        }
	if ( /\/\/\s*gcc_opt:\s*(.*)/ ) {
	    $cc_op .= " $1";
	}
    }

    close $lf or die $!;

    my $cc_error_msg = "";

    run3 ("$cc $cc_op $c_file $link_file",
	  undef, \my $cc_output, \$cc_error_msg);
    ok (! $cc_error_msg, "no errors compiling $c_file and $link_file");
    if ($cc_error_msg) {
	diag ("Compiler error is '$cc_error_msg'");
    }
    return;
}

# There are several errors involving the verbatim copying function
# which are shown up with the test file "ok-verbatim.c".

sub test_verbatim
{
    my $file = "$Bin/warn-verbatim.c";
    my $text = read_text ($file);
    like ($text, qr/=\s*99/, "initializer OK");
    my $hfile = c_to_h_name ($file);
    run3 ("$cfunctions $file", undef, \my $output, \my $errors);
    ok (! $output, "no output with $file");
    ok (-f $hfile, "made $hfile ok");
    my $htext = read_text ($hfile);
    unlike ($htext, qr/99/, "Did not copy 99 into $hfile");
    ok ($errors, "get errors with initializer in verbatim region");
    TODO: {
	local $TODO = 'Fix multiple errors with verbatim copying';
	unlike ($htext, qr/#if\s+0.*#if\s+0/s,
		"No doubled preprocessor conditionals");
    };
}

sub test_line_numbers
{
    my $file = "$Bin/link-arg.c";
    my @lines = read_lines ($file);
    like ($lines[2], qr/int funkyfunc/);
    my ($hfile, $htext) = run_ok ($file);
#    TODO: {
#	local $TODO = 'fix line number errors';
    like ($htext, qr/#line 3/);
    unlike ($htext, qr/#line 4/);
#    };
}

# https://github.com/benkasminbullock/cfunctions/issues/3

sub test_spacing
{
    my $file = "$Bin/spacing.c";
    my ($hfile, $htext) = run_ok ($file);
    like ($htext, qr/kanji_codes_status_t\s+jis2euc/, "preserved space in function declaration");
#    TODO: {
#	local $TODO = 'Do not add spaces before commas or end bracket of function.';
	unlike ($htext, qr/\s[,\)]/, "did not add extra space to function arguments");
#    };
}

sub test_attribute
{
    my $file = "$Bin/attribute.c";
    run3 ("$cfunctions $file", undef, \my $output, \my $errors);
    my $hfile = c_to_h_name ($file);
    ok (-f $hfile, "made hfile");
    TODO: {
	local $TODO = 'fix errors due to __attribute__';
	ok (! $errors, "no errors with __attribute__(()) stuff");
    };
};

sub test_valgrind
{
    run3 ("valgrind $cfunctions ok-*.c", undef, \my $output, \my $errors);
    like ($errors, qr/no leaks are possible/, "all memory is freed");
    unlike ($errors, qr/depends on uninitialised value/, "no bad reads of data");
    rm_h_files ();
}

# Run on a file expecting no output or errors

sub run_ok
{
    my ($file) = @_;
    ok (-f $file, "$file exists");
    my $hfile = c_to_h_name ($file);
    if (-f $hfile) {
	unlink $hfile or die $!;
    }
    my %o;
    run3 ("$cfunctions $file", undef, \my $output, \my $errors);
    ok (-f $hfile, "$hfile made");
    ok (! $output, "no output from $file");
    ok (! $errors, "no errors from $file");
    my $htext = read_text ($hfile);
    return ($hfile, $htext);
}
