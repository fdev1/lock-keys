AC_INIT([lock-keys])
AC_PROG_CC
AC_PATH_PROG([INSTALL], [install])
AC_HEADER_STDC
AC_CHECK_HEADERS(unistd.h string.h pwd.h sys/stat.h sys/types.h)
PKG_CHECK_MODULES([GTK3], gtk+-3.0, ,
	AC_MSG_ERROR([Could not find GTK+ 3.0]))
AC_ARG_ENABLE([autostart],
	AS_HELP_STRING([--enable-autostart], [Add enable/disable autostart option in settings dialog]))
if test x$enable_autostart = xno ; then
	autostart=0
else
	autostart=1
fi
AC_SUBST(autostart)
AC_OUTPUT(Makefile)
