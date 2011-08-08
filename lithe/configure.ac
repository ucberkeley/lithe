# Process this file with autoconf to produce a configure script.
AC_INIT
AC_PREREQ(2.61)

AC_CONFIG_SRCDIR([src/lithe.h])

AC_CANONICAL_SYSTEM

case "${target_os}" in
  linux*)
    echo ===========================================================
    echo Setting up build environment for ${target_cpu} ${target_os}
    echo ===========================================================
    sysdepsdir=sysdeps/unix/sysv/linux/${target_cpu}
    ;;
  *)
    AC_MSG_ERROR("Lithe is currently unsupported on your platform.")
  esac

# Export OS name to Makefiles.
AC_SUBST(sysdepsdir)

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC

# Check for pthread library.
AC_CHECK_LIB([pthread], [pthread_create], [], [AC_MSG_FAILURE([
*** The pthread library is missing or cannot be found.])])

# Check for the ht library.
AC_ARG_WITH([ht],
            AC_HELP_STRING([--with-ht=DIR],
                           [find headers in prefix/include/ht; library in prefix/lib]),
	    [
	    case "$with_ht" in
		 yes) AC_MSG_ERROR([
*** --with-ht requires an argument]) ;;
		 ''|no) AC_MSG_ERROR([
*** The ht library is REQUIRED and cannot be ommited using --without-ht.]) ;;
		 *) CFLAGS="$CFLAGS -I$withval/include"
		    LDFLAGS="$LDFLAGS -L$withval/lib"
		    LIBS="-lht $LIBS" ;;
	    esac
	    ],
	    [])

AC_MSG_CHECKING(for libht)
AC_LINK_IFELSE([AC_LANG_PROGRAM([
		[#include <ht/ht.h>
                void entry() { }]],
		[ht_request(1);])],
		[AC_DEFINE([HAVE_LIBHT], [1],
                          [Define if you have libht])],
		[AC_MSG_RESULT(no)
		 AC_MSG_FAILURE([
*** The ht library is missing or cannot be found.
*** Did you try using --with-ht=DIR?])])
AC_MSG_RESULT(yes)

# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([stdlib.h sys/time.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_C_INLINE
AC_TYPE_PID_T
AC_TYPE_SIZE_T
AC_HEADER_TIME

# Checks for library functions.
AC_FUNC_FORK
AC_FUNC_MALLOC
AC_FUNC_MMAP
AC_FUNC_VPRINTF
AC_CHECK_FUNCS([getpagesize gettimeofday munmap])

AC_CONFIG_FILES([Makefile
		 src/Makefile
		 syncs/Makefile
		 examples/spmd/Makefile
		 examples/barrier/Makefile
		 examples/hierarchy/Makefile
		 examples/futures/Makefile
		 examples/tls/Makefile])

AC_OUTPUT
