#
#      $Id$
#
#########################################################################
#									#
#			   Copyright (C)  2003				#
#	     			Internet2				#
#			   All Rights Reserved				#
#									#
#########################################################################
#
#	File:		Makefile.am
#
#	Author:		Jeff Boote
#			Internet2
#
#	Date:		Thu Dec 18 14:28:42 MST 2003
#
#	Description:

INCLUDES	= $(I2UTILINCS)

bin_PROGRAMS		= sha1test hmac-sha1test
sha1test_SOURCES	= sha1test.c
sha1test_LDADD		= $(I2UTILLIBS) $(MALLOCDEBUGLIBS)
sha1test_DEPENDENCIES	= $(I2UTILLIBDEPS)

hmac_sha1test_SOURCES		= hmac-sha1test.c
hmac_sha1test_LDADD		= $(I2UTILLIBS) $(MALLOCDEBUGLIBS)
hmac_sha1test_DEPENDENCIES	= $(I2UTILLIBDEPS)
