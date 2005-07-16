#
#      $Id$
#
#########################################################################
#									#
#			   Copyright (C)  2002				#
#				Internet2				#
#			   All Rights Reserved				#
#									#
#########################################################################
#
#	File:		configure.ac
#
#	Author:		Jeff Boote
#			Internet2
#
#	Date:		Wed Apr 24 09:32:50 EDT 2002
#
#	Description:	autoconfig input script for I2util build
#
#	Usage:		see bootstrap...
#
#	Environment:
#
#	Files:
#
#
#	Options:
# Process this file with autoconf to produce a configure script.
AC_INIT(I2util, 1.0, owamp-bugs@internet2.edu)
AC_CONFIG_AUX_DIR(config)
AM_INIT_AUTOMAKE(I2util, 1.0, [no-define])
AC_CONFIG_SRCDIR(I2util/ErrLog.c)
AM_CONFIG_HEADER(I2util/config.h)

# Insert local symbols
I2UTILINCS='-I${top_srcdir}'
I2UTIL_dir='${top_srcdir}/I2util'
I2UTILLDFLAGS="-L$I2UTIL_dir"
I2UTILLIBS="$I2UTILLDFLAGS -lI2util"
I2UTILLIBDEPS="$I2UTIL_dir/libI2util.a"

AC_SUBST(I2UTILINCS)
AC_SUBST(I2UTILLIBS)
AC_SUBST(I2UTILLIBDEPS)

# Check for kernel support
I2_RANDOMDEV

# Checks for programs.
AC_PROG_CC
AC_PROG_RANLIB

# Checks for libraries.

# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([errno.h stdlib.h string.h limits.h stddef.h paths.h])

# Checks for typedefs, structures, and compiler characteristics.
I2_C___ATTRIBUTE__
AC_C_CONST
AC_TYPE_SIZE_T
AC_STRUCT_TM
I2_C_SYSLOG_NAMES
I2_C_SYSLOG_PERROR

# Checks for library functions.
AC_FUNC_MALLOC
AC_FUNC_STRFTIME
AC_CHECK_FUNCS([memset strdup strerror strtol strtoul])

AC_SUBST(ac_aux_dir)
AC_OUTPUT([Makefile I2util/Makefile aespasswd/Makefile doc/Makefile],
	[test -z "$CONFIG_HEADERS" || echo timestamp > I2util/stamp-h.in])
