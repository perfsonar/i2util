#
#      $Id$
#
#########################################################################
#									#
#			   Copyright (C)  2002				#
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
#	Date:		Wed Apr 24 09:50:44 EDT 2002
#
#	Description:	libI2util build description.

#include $(top_srcdir)/config/Make-rules

INCLUDES	=	-I$(top_builddir) -I$(top_srcdir)

lib_LIBRARIES		= libI2util.a
libI2util_a_SOURCES	= ErrLog.c ErrLogImmediate.c ErrLogSyslog.c \
			Pthread.c mach_dep.c mach_dep.h table.c \
			random.c io.c saddr.c md5.c readpassphrase.c hex.c \
			conf.c addr.c sha1.c hmac-sha1.c pbkdf2.c
EXTRA_DIST	=	Pthread.h errlog.h errlogimmediate.h errlogsyslog.h \
			mach_dep.h io.h random.h saddr.h table.h md5.h \
			readpassphrase.h conf.h hex.h util.h addr.h sha1.h \
			sha1P.h hmac-sha1.h pbkdf2.h
