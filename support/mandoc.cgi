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

$CGI::POST_MAX=1024 * 100;  # max 100K posts
$CGI::DISABLE_UPLOADS = 1;  # no uploads

# Turn off buffered I/O.
$|=0;

# man program (must take a filename as an arg.)
my $ManExe	=	'/usr/bin/man';
my $ManConv	=	'/home/boote/public_html/cgi/man2html';

# args to man2html
my @m2hargs	=	("-compress",
			-cgiurl =>	'${title}.man.html',
			-botm	=>	0,
			-topm	=>	0,
			);
$q = new CGI;

#
# Determine which "root"
# setup ARGV to "require" right into man2html
@ARGV = @m2hargs;

print $q->header(	-type=>'text/html',
			-expires=>'+1d');

# use open to fork and exec man(1)
open(MAN,"-|") or exec($ManExe, $q->path_translated);

# If open succeeded, then we are the read end of the pipe and continue
# on to read the output of man(1) and send it out to the client.
$Man2Html::InFH = \*MAN;
require $ManConv or
	die("Unable to run man2html: $!");
