dnl
dnl	$Id$
dnl

dnl
dnl	This macro is used to test for a kernel random device
dnl

AC_DEFUN(I2_RANDOMDEV, [
AC_MSG_CHECKING(for kernel random dev)
AC_CACHE_VAL(i2_cv_randomdev, [
	i2_cv_randomdev="no"
	for dev in /dev/urandom /dev/srandom /dev/random /dev/srnd /dev/rnd; do
	    if test -c "$dev"; then
	        i2_cv_randomdev=$dev
		break
	    fi
	done
])
if test "$i2_cv_randomdev" != "no"; then
  AC_DEFINE_UNQUOTED(I2_RANDOMDEV_PATH, "$i2_cv_randomdev", [path to kernel random device])
fi
AC_MSG_RESULT($i2_cv_randomdev)
])
