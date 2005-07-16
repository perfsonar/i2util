dnl
dnl	$Id$
dnl

dnl
dnl	This macro is used to test for the syslog LOG_PERROR constant
dnl

AC_DEFUN(I2_C_SYSLOG_PERROR, [
AC_MSG_CHECKING(for syslog perror availability)
AC_CACHE_VAL(ac_cv_syslogperror, [
AC_TRY_COMPILE([
#include <stdlib.h>
#include <syslog.h>
],
[
#ifndef LOG_PERROR
#error  "No LOG_PERROR defined"
#endif
],
ac_cv_syslogperror=yes,
ac_cv_syslogperror=no)])
if test "$ac_cv_syslogperror" = "yes"; then
  AC_DEFINE(HAVE_SYSLOG_PERROR, 1, [define if your compiler has syslog names])
fi
AC_MSG_RESULT($ac_cv_syslogperror)
])
