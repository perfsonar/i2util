dnl
dnl	$Id$
dnl

dnl
dnl	This macro is used to test for the syslog prioritynames global array
dnl

AC_DEFUN(I2_C_SYSLOG_NAMES, [
AC_MSG_CHECKING(for syslog names)
AC_CACHE_VAL(ac_cv_syslognames, [
AC_TRY_COMPILE([
#include <stdlib.h>
#define	SYSLOG_NAMES
#include <syslog.h>
],
[
static void foo(void);

static void
foo(void)
{
  CODE	*ptr = &prioritynames[0];

  while(ptr->c_name)
      ptr++;

  ptr = &facilitynames[0];
  while(ptr->c_name)
      ptr++;

  exit(0);
}
],
ac_cv_syslognames=yes,
ac_cv_syslognames=no)])
if test "$ac_cv_syslognames" = "yes"; then
  AC_DEFINE(HAVE_SYSLOG_NAMES, 1, [define if your compiler has syslog names])
fi
AC_MSG_RESULT($ac_cv_syslognames)
])
