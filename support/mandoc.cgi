#!/usr/bin/perl
#
#      $Id$
#
#########################################################################
#                                                                       #
#                          Copyright (C)  2004                          #
#                               Internet2                               #
#                          All Rights Reserved                          #
#                                                                       #
#########################################################################
#
#       File:           mandoc.cgi
#
#       Author:         Jeff Boote
#                       Internet2
#
#       Date:           Fri Feb 20 16:36:35 MST 2004
#
#       Description:
#
#       Usage:
#
#       Environment:
#
#       Files:
#
#       Options:
use CGI;
use CGI::Carp 'fatalsToBrowser';
use File::Basename;

$CGI::POST_MAX=1024 * 100;  # max 100K posts
$CGI::DISABLE_UPLOADS = 1;  # no uploads

# Add the directory this script lives in to the INC list.
unshift(@INC,substr($0,$[,rindex($0,'/',length($0)))."/.");

# Turn off buffered I/O.
$|=0;

############################################################
# CONFIGURE HERE
#
use constant InstallAsAction => 1;	# True or False

# man program (must take a filename as an arg.)
my $ManExe	=	'/usr/bin/man';
my $ManConv	=	'man2html';

# args to man2html
my @m2hargs	=	("-compress",
			-cgiurl =>	'${title}.man.html',
			-botm	=>	0,
			-topm	=>	0,
			);
# End configure
############################################################

$q = new CGI;

#
# Determine which "root"
# setup ARGV to "require" right into man2html
@ARGV = @m2hargs;

print $q->header(	-type=>'text/html',
			-expires=>'+1d');

if(InstallAsAction && !defined($ENV{'REDIRECT_URL'})){
	print $q->start_html(	-title=>'Invalid use of mandoc.cgi',
				-author=>'boote@internet2.edu');
	print $q->h1("Invalid use of mandoc.cgi!");
	print $q->p("mandoc.cgi is intended to be used as the target of ",
		"an Apache \"Action\" directive. It is invalid in other",
		"context.");
	print $q->p("Please report problems to",
		$q->a({href=>'mailto:'.$ENV{'SERVER_ADMIN'}},
					$ENV{'SERVER_ADMIN'}),
		".");
	print $q->end_html;
	exit(1);
}

if(!-r $q->path_translated){
	print $q->start_html(	-title=>'Error',
				-author=>'boote@internet2.edu');
	print $q->h1("Unable to read: ",$q->path_info);
	print $q->p("Unable to read: ",$q->path_info);
	print $q->p("Please report problems to",
		$q->a({href=>'mailto:'.$ENV{'SERVER_ADMIN'}},
					$ENV{'SERVER_ADMIN'}),
		".");
	print $q->end_html;
	exit(1);
}
	
# chdir so man '.so' includes will work.
my $path = dirname($q->path_translated);
chdir($path) || die "Can't cd to $path: $!\n";

# use open to fork and exec man(1)
open(MAN,"-|") or exec($ManExe, $q->path_translated);

# If open succeeded, then we are the read end of the pipe and continue
# on to read the output of man(1) and send it out to the client.
$Man2Html::InFH = \*MAN;
require $ManConv or
	die("Unable to run man2html: $!");
