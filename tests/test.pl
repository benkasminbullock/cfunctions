#!/home/ben/software/install/bin/perl

# This (test.pl) is an input file for the script `configure' in the
# top level directory.

# The output is a test script for Cfunctions written in Perl.
 
use warnings;
use strict;
use FindBin '$Bin';
use Test::More;

my $cfunctions = "$Bin/../src/cfunctions";

if (! -x $cfunctions) {
    die "No $cfunctions";
}

my $cc         = "cc";
my $GNU_c      = 1;

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

# Remove header files, backup header files and other generated files
# from the current directory.

sub rm_h_files()
{
    opendir(THISDIR, ".") or die $!;
    my @h_files = grep ( /^.*\.h~*$/, readdir THISDIR);
    closedir THISDIR or die $!;

    foreach my $h_file (@h_files)
    {
        unlink ( $h_file );
    }
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

    while ( <TEST_OUT> )
    {
	$error_msg .= $_;
    }
    close TEST_OUT;
    ok (! $error_msg, "no errors with $cfunctions $c_file");

# Now for the real test: try to compile and link without warnings
# using the generated header file.

    $test = "PASSED";

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

    while ( <LINK_FILE> )
    {
        if ( /\/\/\s*options:\s*(.*)/ )
        {
            $cc_op .= $1;
        }
        if ( $GNU_c )
        {
            if ( /\/\/\s*gcc_opt:\s*(.*)/ )
            {
                $gcc_op .= $1;
            }
        }
    }

    close LINK_FILE;

#    note "$cc $cc_op $gcc_op $c_file $link_file: "; 

# The following `2>&1' directs `stderr' to Perl and `stdout' to
# `/dev/null' (according to the Bash manual).

    $error_msg = "";

    open ( CC_OUTPUT, 
           "$cc $cc_op $gcc_op $c_file $link_file 2>&1 >/dev/null |" );

    while ( <CC_OUTPUT> )
    {
        if ( /.+/ )
        {
            $error_msg .= $_;
        }
    }

    close CC_OUTPUT;

    ok (! $error_msg, "no errors compiling $c_file and $link_file");
    if ($error_msg) {
	note ("Error is '$error_msg'");
    }
    return;
}


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

    done_testing ();
}

main ();
exit (0);
