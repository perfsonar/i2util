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

MACH_SOURCES	= mach_dep.c mach_dep.h

lib_LIBRARIES		= libI2util.a
libI2util_a_SOURCES	= utilP.h ErrLog.c ErrLogImmediate.c ErrLogSyslog.c \
			random.c io.c saddr.c md5.c readpassphrase.c hex.c \
			conf.c addr.c sha1.c hmac-sha1.c pbkdf2.c table.c \
			$(MACH_SOURCES)

I2utilincludedir = $(pkgincludedir)
I2utilinclude_HEADERS	= Pthread.h errlog.h errlogimmediate.h errlogsyslog.h \
			mach_dep.h io.h random.h saddr.h table.h md5.h \
			readpassphrase.h conf.h hex.h util.h addr.h sha1.h \
			sha1P.h hmac-sha1.h pbkdf2.h

if I2THREADS_ENABLE
MACH_SOURCES	+= Pthread.c
endif
# 
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
# 
