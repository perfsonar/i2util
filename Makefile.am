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
#	Date:		Wed Apr 24 09:49:28 EDT 2002
#
#	Description:	toplevel build for I2util library
#

#include $(top_srcdir)/config/Make-rules

ACLOCAL_AMFLAGS = -I config
AUTOMAKE_OPTIONS = foreign

SUBDIRS = I2util aespasswd pfstore test doc

# uncomment when automake is unbroken 1.5 doesn't work with this...
# (then remove these from DIST_EXTRA in subdir)
#nobase_include_HEADERS = I2util/util.h I2util/Pthread.h I2util/config.h \
#		I2util/errlog.h I2util/errlogimmediate.h I2util/errlogsyslog.h \
#		I2util/options.h I2util/table.h I2util/random.h I2util/io.h \
#		I2util/saddr.h
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
