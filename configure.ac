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

AC_INIT(I2util, 4.3.0, owamp-bugs@internet2.edu, I2util, https://github.com/perfsonar/i2util/)
AC_CONFIG_AUX_DIR(config)
AC_CONFIG_MACRO_DIR(config)
AM_INIT_AUTOMAKE([no-define])
AC_CONFIG_SRCDIR(I2util/ErrLog.c)
AM_CONFIG_HEADER(I2util/config.h)

# catch unexpanded macros
m4_pattern_forbid([^I2_])

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

AC_CANONICAL_HOST
AC_DEFINE(_GNU_SOURCE, 1, "Use glibc features.")
AC_SYS_LARGEFILE
AM_CONDITIONAL(I2THREADS_ENABLE, false)

AS_CASE($host,
	*-*-solaris*,[dnl
	AC_DEFINE(__EXTENSIONS__, 1, "get decl for sockaddr_storage on Solaris")dnl
	AC_DEFINE(_XOPEN_SOURCE, 500, "get decl for msg_control on Solaris")])

# Checks for libraries.
AC_SEARCH_LIBS(getaddrinfo, [socket nsl])
AC_SEARCH_LIBS(gethostbyname, nsl)
AC_SEARCH_LIBS(socket, socket)

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
AC_CHECK_MEMBERS([struct sockaddr.sa_len], , ,
                 [#include <sys/types.h>
                  #include <sys/socket.h>])
AC_CHECK_DECLS([sys_nerr, sys_errlist], , ,
               [#include <stdlib.h>
                #include <stdio.h>])

# Checks for library functions.
AC_FUNC_STRFTIME
AC_CHECK_FUNCS([malloc memset strdup strerror strtol strtoul socket getaddrinfo])

# Check for MAN2HTML. The manpages will be compiled to html files if it's
# found.
AC_CHECK_PROGS([MAN2HTML], [man2html])
AS_IF([test -n "${MAN2HTML}"],
    [AC_DEFINE(MAN2HTML, 1, [MAN2HTML man-page converter])dnl
    do_man2html=true])
AM_CONDITIONAL([HAVE_MAN2HTML], test x$do_man2html = xtrue)

AC_SUBST(ac_aux_dir)
AC_OUTPUT([Makefile I2util/Makefile aespasswd/Makefile pfstore/Makefile doc/Makefile test/Makefile I2util.spec],
	[test -z "$CONFIG_HEADERS" || echo timestamp > I2util/stamp-h.in])
